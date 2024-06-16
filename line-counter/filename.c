#include "linecounter.h"

LPSTR getProgramFileName(const LPCSTR argument) {
	if(!argument) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	size_t length = strlen(argument);

	if(length < 1) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	size_t lastBackslash = 0;
	size_t lastDot = length;

	for(size_t i = 0; i < length; i++) {
		size_t index = length - i - 1;

		if(argument[index] == '\\' || argument[index] == '/') {
			lastBackslash = index + 1;
			break;
		}
	}

	for(size_t i = 0; i < length; i++) {
		size_t index = length - i - 1;

		if(argument[index] == '.') {
			lastDot = index;
			break;
		}
	}

	length = lastDot - lastBackslash;

	if(length < 1) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return NULL;
	}

	BOOL hasSpace = FALSE;

	for(size_t i = lastBackslash; i <= lastDot; i++) {
		if(argument[i] == ' ' || argument[i] == '\n' || argument[i] == '\r' || argument[i] == '\t') {
			hasSpace = TRUE;
			length += 2;
			break;
		}
	}

	LPSTR buffer = LocalAlloc(LMEM_FIXED, (length + 1) * sizeof(CHAR));

	if(!buffer) {
		return NULL;
	}

	for(size_t i = lastBackslash; i <= lastDot; i++) {
		buffer[i - lastBackslash + hasSpace] = argument[i];
	}

	if(hasSpace) {
		buffer[0] = '"';
		buffer[length - 1] = '"';
	}

	buffer[length] = 0;
	return buffer;
}
