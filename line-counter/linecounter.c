#include <stdio.h>
#include <khopanwin32.h>
#include <khopanstring.h>
#include <ShlObj_core.h>
#include "linecounter.h"

static BOOL recursiveLineCount(const LPCWSTR fileExtension, BOOL recursiveSearch, BOOL separateFiles, const LPCWSTR searchPath, size_t* lines);

int countLine(const LPCWSTR fileExtension, BOOL recursiveSearch, BOOL separateFiles, const LPCWSTR searchPath) {
	DWORD pathFolderCurrentDirectoryLength = GetCurrentDirectoryW(0, NULL);

	if(!pathFolderCurrentDirectoryLength) {
		KHWin32ConsoleErrorW(GetLastError(), L"GetCurrentDirectoryW");
		return 1;
	}

	LPWSTR pathFolderCurrentDirectory = LocalAlloc(LMEM_FIXED, pathFolderCurrentDirectoryLength * sizeof(WCHAR));

	if(!pathFolderCurrentDirectory) {
		KHWin32ConsoleErrorW(GetLastError(), L"LocalAlloc");
		return 1;
	}

	if(!GetCurrentDirectoryW(pathFolderCurrentDirectoryLength, pathFolderCurrentDirectory)) {
		KHWin32ConsoleErrorW(GetLastError(), L"GetCurrentDirectoryW");
		return 1;
	}

	LPWSTR pathFolderRawSearch = KHFormatMessageW(L"%ws\\%ws", pathFolderCurrentDirectory, searchPath);
	LocalFree(pathFolderCurrentDirectory);

	if(!pathFolderRawSearch) {
		KHWin32ConsoleErrorW(ERROR_FUNCTION_FAILED, L"KHFormatMessageW");
		return 1;
	}

	WCHAR path[MAX_PATH];
	memset(path, 0, MAX_PATH);
	size_t rawSearchLength = wcslen(pathFolderRawSearch);

	for(size_t i = 0; i < min(rawSearchLength, MAX_PATH); i++) {
		path[i] = pathFolderRawSearch[i];
	}

	LocalFree(pathFolderRawSearch);

	if(!PathResolve(path, NULL, 0)) {
		KHWin32ConsoleErrorW(GetLastError(), L"PathResolve");
		return 1;
	}

	size_t lines = 0;

	if(!recursiveLineCount(fileExtension, recursiveSearch, separateFiles, path, &lines)) {
		return 1;
	}

	if(!separateFiles) {
		printf("%s: %llu\n", lines == 1 ? "Line" : "Lines", lines);
	}

	return 0;
}

static BOOL checkFileExtension(const LPCWSTR filePath, const LPCWSTR extension) {
	if(!extension) {
		return TRUE;
	}

	size_t extensionLength = wcslen(extension);

	if(!extensionLength) {
		return TRUE;
	}

	size_t fileLength = wcslen(filePath);

	if(fileLength <= extensionLength) {
		return FALSE;
	}

	size_t offset = fileLength - extensionLength;

	if(filePath[offset - 1] != L'.') {
		return FALSE;
	}

	for(size_t i = 0; i < extensionLength; i++) {
		if(towlower(filePath[i + offset]) != towlower(extension[i])) {
			return FALSE;
		}
	}

	return TRUE;
}

static BOOL countFileLines(const LPCWSTR fileName, BOOL separateFiles, size_t* lines) {
	LPWSTR longPath = KHFormatMessageW(L"\\\\?\\%ws", fileName);

	if(!longPath) {
		KHWin32ConsoleErrorW(ERROR_FUNCTION_FAILED, L"FormatMessageW");
		return FALSE;
	}

	HANDLE file = CreateFileW(longPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	LocalFree(longPath);

	if(file == INVALID_HANDLE_VALUE) {
		KHWin32ConsoleErrorW(GetLastError(), L"CreateFileW");
		return FALSE;
	}
	
	LARGE_INTEGER integer;
	
	if(!GetFileSizeEx(file, &integer)) {
		KHWin32ConsoleErrorW(GetLastError(), L"GetFileSizeEx");
		CloseHandle(file);
		return FALSE;
	}

	LPSTR buffer = LocalAlloc(LMEM_FIXED, integer.LowPart);

	if(!buffer) {
		KHWin32ConsoleErrorW(GetLastError(), L"LocalAlloc");
		CloseHandle(file);
		return FALSE;
	}

	DWORD bytesRead;

	if(!ReadFile(file, buffer, integer.LowPart, &bytesRead, NULL)) {
		KHWin32ConsoleErrorW(GetLastError(), L"ReadFile");
		LocalFree(buffer);
		CloseHandle(file);
		return FALSE;
	}

	CloseHandle(file);

	if(integer.LowPart != bytesRead) {
		KHWin32ConsoleErrorW(ERROR_FUNCTION_FAILED, L"ReadFile");
		LocalFree(buffer);
		return FALSE;
	}

	size_t totalLines = 0;

	for(size_t i = 0; i < integer.LowPart; i++) {
		if(buffer[i] == '\n') {
			totalLines++;
		}
	}

	LocalFree(buffer);

	if(!separateFiles) {
		(*lines) += totalLines;
		return TRUE;
	}

	wprintf(L"%ws: %llu %S\n", fileName, totalLines, totalLines == 1 ? "line" : "lines");
	return TRUE;
}

static BOOL recursiveLineCount(const LPCWSTR fileExtension, BOOL recursiveSearch, BOOL separateFiles, const LPCWSTR searchPath, size_t* lines) {
	LPWSTR search = KHFormatMessageW(L"%ws\\*", searchPath);

	if(!search) {
		KHWin32ConsoleErrorW(ERROR_FUNCTION_FAILED, L"KHFormatMessageW");
		return FALSE;
	}

	WIN32_FIND_DATAW data;
	HANDLE searchHandle = FindFirstFileW(search, &data);
	LocalFree(search);

	if(searchHandle == INVALID_HANDLE_VALUE) {
		KHWin32ConsoleErrorW(GetLastError(), L"FindFirstFileW");
		return FALSE;
	}

	BOOL noError = FALSE;
	BOOL returnValue = FALSE;

	do {
		if(!wcscmp(data.cFileName, L".") || !wcscmp(data.cFileName, L"..")) {
			continue;
		}

		LPWSTR pathName = KHFormatMessageW(L"%ws\\%ws", searchPath, data.cFileName);

		if(!pathName) {
			KHWin32ConsoleErrorW(ERROR_FUNCTION_FAILED, L"KHFormatMessageW");
			goto closeSearchHandle;
		}

		if(!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			if(!checkFileExtension(data.cFileName, fileExtension)) {
				LocalFree(pathName);
				continue;
			}

			BOOL value = countFileLines(pathName, separateFiles, lines);

			if(!value) {
				returnValue = value;
				goto closeSearchHandle;
			}

			LocalFree(pathName);
			continue;
		}

		if(recursiveSearch) {
			BOOL value = recursiveLineCount(fileExtension, recursiveSearch, separateFiles, pathName, lines);
			LocalFree(pathName);

			if(!value) {
				returnValue = value;
				goto closeSearchHandle;
			}
		}
	} while(noError = FindNextFileW(searchHandle, &data));

	if(!noError && GetLastError() != ERROR_NO_MORE_FILES) {
		KHWin32ConsoleErrorW(GetLastError(), L"FindNextFileW");
		goto closeSearchHandle;
	}

	returnValue = TRUE;
closeSearchHandle:
	FindClose(searchHandle);
	return returnValue;
}
