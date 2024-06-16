#include <stdio.h>
#include <khopanwin32.h>
#include "linecounter.h"

typedef struct {
	char* option;
	char* description;
} linecounter_option;

static linecounter_option globalOptionList[] = {
	{"-h",             "Display this help information"},
	{"-x <extension>", "Specify the file extension when searching"},
	{"-r",             "Search the directory recursively"},
	{"-s",             "Print the number of lines for each file"}
};

void printHelpInformation(const LPCSTR programPath) {
	LPSTR programName = getProgramFileName(programPath);

	if(!programName) {
		KHWin32ConsoleErrorW(GetLastError(), L"getProgramFileName");
		return;
	}

	printf("Usage: %s [options] <path> [options]\n\nOptions:\n", programName);
	LocalFree(programName);
	size_t optionLength = sizeof(globalOptionList) / sizeof(globalOptionList[0]);
	size_t longestOptionLength = 0;

	for(size_t i = 0; i < optionLength; i++) {
		size_t length = strlen(globalOptionList[i].option);

		if(length > longestOptionLength) {
			longestOptionLength = length;
		}
	}

	longestOptionLength += 3;

	for(size_t x = 0; x < optionLength; x++) {
		printf("\t%s", globalOptionList[x].option);

		for(size_t y = 0; y < longestOptionLength - strlen(globalOptionList[x].option); y++) {
			putchar(' ');
		}

		printf("%s\n", globalOptionList[x].description);
	}
}
