#include <stdio.h>
#include <khopanwin32.h>
#include <khopanstring.h>
#include "linecounter.h"

int main(int argc, char** argv) {
	if(argc < 2) {
		printf("Error: No command-line option\n\n");
		printHelpInformation(argv[0]);
		return 0;
	}

	LPSTR fileExtension = NULL;
	BOOL recursiveSearch = FALSE;
	BOOL separateFiles = FALSE;
	LPSTR searchPath = NULL;

	for(int i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "-h")) {
			printHelpInformation(argv[0]);
			return 0;
		}

		if(!strcmp(argv[i], "-x")) {
			if(i == argc - 1) {
				printf("Error: Option -x is missing the <extension> parameter\n");
				return 1;
			}

			i++;
			fileExtension = argv[i];
			continue;
		}

		if(!strcmp(argv[i], "-r")) {
			if(recursiveSearch) {
				printf("Warning: Duplicate option: -r\n");
			}

			recursiveSearch = TRUE;
			continue;
		}

		if(!strcmp(argv[i], "-s")) {
			if(separateFiles) {
				printf("Warning: Duplicate option: -s\n");
			}

			separateFiles = TRUE;
			continue;
		}

		if(!searchPath) {
			searchPath = argv[i];
			continue;
		}

		printf("Unknown option: %s\n", argv[i]);
		return 1;
	}

	LPWSTR wideExtension = !fileExtension ? NULL : KHFormatMessageW(L"%S", fileExtension);

	if(fileExtension && !wideExtension) {
		KHWin32ConsoleErrorW(ERROR_FUNCTION_FAILED, L"KHFormatMessageW");
		return 1;
	}

	LPWSTR wideSearchPath = !searchPath ? NULL : KHFormatMessageW(L"%S", searchPath);
	int returnValue = 1;

	if(searchPath && !wideSearchPath) {
		KHWin32ConsoleErrorW(ERROR_FUNCTION_FAILED, L"KHFormatMessageW");
		goto freeWideExtension;
	}

	returnValue = countLine(wideExtension, recursiveSearch, separateFiles, wideSearchPath);

	if(searchPath) {
		LocalFree(wideSearchPath);
	}

freeWideExtension:
	if(fileExtension) {
		LocalFree(wideExtension);
	}

	return returnValue;
}
