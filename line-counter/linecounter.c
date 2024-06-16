#include <stdio.h>
#include <khopanwin32.h>
#include <khopanstring.h>
#include "linecounter.h"

int countLine(const LPCWSTR fileExtension, BOOL recursiveSearch, BOOL separateFiles, const LPCWSTR searchPath) {
	LPWSTR search = KHFormatMessageW(L"%s\\*", searchPath);

	if(!search) {
		KHWin32ConsoleErrorW(ERROR_FUNCTION_FAILED, L"KHFormatMessageW");
		return 1;
	}

	WIN32_FIND_DATAW data;
	printf("Search: %ws\n", search);
	HANDLE searchHandle = FindFirstFileW(search, &data);
	LocalFree(search);

	if(searchHandle == INVALID_HANDLE_VALUE) {
		KHWin32ConsoleErrorW(GetLastError(), L"FindFirstFileW");
		return 1;
	}

	BOOL noError = FALSE;

	do {
		wprintf(L"%d %ws\n", data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY, data.cFileName);
	} while(noError = FindNextFileW(searchHandle, &data));

	if(!noError && GetLastError() != ERROR_NO_MORE_FILES) {
		KHWin32ConsoleErrorW(GetLastError(), L"FindNextFileW");
		return 1;
	}

	FindClose(searchHandle);
	return 0;
}
