#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

typedef struct LargeNumber LargeNumber;

// Creates a large number that represents the smallest possible number with the specified number of
// digits.
LargeNumber* SmallestWithDigits(size_t digits);

// Frees the memory used by a number
void FreeNumber(LargeNumber* number);

// Makes a copy of a number
LargeNumber* CopyNumber(LargeNumber* number);

// Increments a large number in-place
void Increment(LargeNumber* number);

// Multiplies the digits of a large number, resulting in another large number
LargeNumber* MultiplyDigits(LargeNumber* number);

// Gets the number of digits of a large number
size_t NumberOfDigits(LargeNumber* number);

// Checks if a number is in the Multiplicative Persistence search space
// A number isn't on the search space if:
// One of its digits is zero
// Its digits aren't in ascending order
bool IsInSearchSpace(LargeNumber* number);

// Prints a number to a stream
void FPrintNumber(FILE* file, LargeNumber* number);
