#pragma once

#include <stdio.h>

typedef struct LargeNumber LargeNumber;

// Creates a large number that represents the smallest possible number within the search space
// that has the specified number of digits
LargeNumber* SmallestWithDigits(size_t digits);

// Frees the memory used by a number
void FreeNumber(LargeNumber* number);

// Makes a copy of a number
LargeNumber* CopyNumber(LargeNumber* number);

// Advances a large number in-place to the next number in the search space
void Increment(LargeNumber* number);

// Multiplies the digits of a large number, resulting in another large number
LargeNumber* MultiplyDigits(LargeNumber* number);

// Gets the number of digits of a large number
size_t NumberOfDigits(LargeNumber* number);

// Prints a number to a stream
void FPrintNumber(FILE* file, LargeNumber* number);
