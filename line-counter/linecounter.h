#pragma once

#include <Windows.h>

LPSTR getProgramFileName(const LPCSTR argument);
void printHelpInformation(const LPCSTR programPath);
int countLine(const LPCWSTR fileExtension, BOOL recursiveSearch, BOOL separateFiles, const LPCWSTR searchPath);
