#include <stdio.h>
#include <khopanwin32.h>
#include "linecounter.h"

int main(int argc, char** argv) {
	if(argc) {
		LPSTR programName = getProgramFileName(argv[0]);

		if(!programName) {
			KHWin32ConsoleErrorW(GetLastError(), L"getProgramFileName");
			return 1;
		}

		printf("Usage: %s [options] <path>\n", programName);
		LocalFree(programName);
		return 0;
	}

	return 0;
}
