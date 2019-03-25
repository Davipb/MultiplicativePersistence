#pragma once

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// A type that can hold unsigned integers of arbitrarily large sizes, bounded only
// by the vailable memory
typedef struct LargeNumber LargeNumber;

// Creates a large number that represents the smallest possible number within the search space
// that has the specified number of digits
LargeNumber* SmallestWithDigits(size_t digits);

// Frees the memory used by a number
void FreeNumber(LargeNumber* number);

// Makes a copy of a number
LargeNumber* CopyNumber(LargeNumber* number);

// Advances a large number in-place to the next number in the search space
// Returns 'true' if the amount of digits of the number was increased
bool Increment(LargeNumber* number);

// Multiplies the digits of a large number, resulting in another large number
LargeNumber* MultiplyDigits(LargeNumber* number);

// Compares two large numbers
// Returns -1 if a is smaller than b
// Returns 0 if a is equal to b
// Returns 1 if a is greater than b
int8_t Compare(LargeNumber* a, LargeNumber* b);

// Gets the number of digits of a large number
size_t NumberOfDigits(LargeNumber* number);

// Prints a large number to a stream
void FPrintNumber(FILE* file, LargeNumber* number);

// Reads a large number from a stream
// Returns NULL if the reading failed for any reason
LargeNumber* FScanNumber(FILE* file);

// Reads a large number from a string
// Returns NULL if the reading failed for any reason
LargeNumber* SScanNumber(char* string);
