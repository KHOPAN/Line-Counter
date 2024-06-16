#include <stdio.h>
#include "linecounter.h"

int main(int argc, char** argv) {
	if(argc < 2) {
		printHelpInformation(argv[0]);
		return 0;
	}

	for(int i = 1; i < argc; i++) {
		if(!strcmp(argv[i], "-h")) {
			printHelpInformation(argv[0]);
			return 0;
		}

		printf("Unknown option: %s\n", argv[i]);
		return 1;
	}

	return 0;
}
