#include <stdio.h>
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

	return 0;
}
