#include "Number.h"

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#define BASE 10

typedef uint8_t Digit;

struct LargeNumber
{
    // The size of the number in digits
    size_t Size;

    // The digits of the number, from least significant to most significant
    // The base-10 number 12345 would be stored as { 5, 4, 3, 2, 1 }
    Digit* Digits;
};

// Creates a new large number that is initialized to zero
// size: The initial size of the number, in digits
static LargeNumber* NewNumber(size_t size)
{
    LargeNumber* number = malloc(sizeof(LargeNumber));
    number->Size = size;
    number->Digits = calloc(size, sizeof(Digit));
    return number;
}

void FreeNumber(LargeNumber* number)
{
    if (number == NULL) return;
    free(number->Digits);
    free(number);
}

// Changes the size of a number
// If the new size is longer than the current size of the number, its value isn't changed
// If the new size is shorter than the current size of the number, its value is truncated
static void ResizeNumber(LargeNumber* number, size_t newSize)
{
    if (number->Size == newSize) return;

    number->Digits = realloc(number->Digits, newSize);

    for (size_t i = number->Size; i < newSize; i++)
        number->Digits[i] = 0;

    number->Size = newSize;
}

// Removes all zeroes left at the end of a number, so that its Size matches exactly
// the number of digits it has.
static void TrimNumber(LargeNumber* number)
{
    if (number->Size == 0) return;

    size_t newSize = number->Size;

    while (newSize > 0 && number->Digits[newSize - 1] == 0)
        newSize--;

    ResizeNumber(number, newSize);
}

// Gets a digit from a number, returning 0 if the digit position
// is outside the number.
static Digit GetDigit(LargeNumber* number, size_t index)
{
    if (index >= number->Size)
        return 0;

    return number->Digits[index];
}

// Sets the digit of a number, automatically resizing the number if it isn't
// big enough
static void SetDigit(LargeNumber* number, size_t index, Digit digit)
{
    if (index >= number->Size)
    {
        // Don't bother resizing the number if we would've set the digit to a 0 anyway
        if (digit == 0) return;

        ResizeNumber(number, index + 1);
    }

    number->Digits[index] = digit;
}

LargeNumber* CopyNumber(LargeNumber* number)
{
    LargeNumber* copy = NewNumber(number->Size);
    for (size_t i = 0; i < number->Size; i++)
        SetDigit(copy, i, GetDigit(number, i));

    return copy;
}

// Makes a large number with the specified numeric value
static LargeNumber* MakeLarge(uintmax_t value)
{
    LargeNumber* number = NewNumber(0);

    size_t index = 0;
    while (value != 0)
    {
        SetDigit(number, index++, value % BASE);
        value /= BASE;
    }

    TrimNumber(number);
    return number;
}

LargeNumber* SmallestWithDigits(size_t digits)
{
    LargeNumber* result = NewNumber(digits);
    SetDigit(result, digits - 1, 1);
    return result;
}

void Increment(LargeNumber* number)
{
    SetDigit(number, 0, GetDigit(number, 0) + 1);

    size_t i = 0;
    while (GetDigit(number, i) >= BASE)
    {
        Digit carry = GetDigit(number, i) / BASE;
        SetDigit(number, i, GetDigit(number, i) % BASE);
        SetDigit(number, i + 1, GetDigit(number, i + 1) + carry);

        i++;
    }
}

// Multiplies a large number by the value of a digit in-place
static void Multiply(LargeNumber* number, Digit digit)
{
    if (digit == 0)
    {
        ResizeNumber(number, 0);
        return;
    }

    Digit carry = 0;
    for (size_t i = 0; i < number->Size; i++)
    {
        Digit value = GetDigit(number, i) * digit + carry;
        carry = value / BASE;
        SetDigit(number, i, value % BASE);
    }

    if (carry != 0)
        SetDigit(number, number->Size, carry);
}

LargeNumber* MultiplyDigits(LargeNumber* number)
{
    LargeNumber* acc = MakeLarge(GetDigit(number, 0));

    for (size_t i = 1; i < number->Size; i++)
        Multiply(acc, GetDigit(number, i));
    
    return acc;
}

size_t NumberOfDigits(LargeNumber* number)
{
    TrimNumber(number);
    if (number->Size <= 1) return 1;
    return number->Size;
}

// Checks if a number is in the Multiplicative Persistence search space
// A number isn't on the search space if:
// One of its digits is zero
// Its digits aren't in ascending order
bool IsInSearchSpace(LargeNumber* number)
{
    TrimNumber(number);
    for (size_t i = 0; i < number->Size; i++)
    {
        if (GetDigit(number, i) == 0) 
            return false;

        if (GetDigit(number, i) < GetDigit(number, i + 1)) 
            return false;

        if (GetDigit(number, i) == 1 && BASE != 2)
            return false;
    }

    return true;
}

void FPrintNumber(FILE* file, LargeNumber* number)
{
    if (number->Size == 0)
    {
        fprintf(file, "0");
        return;
    }

    for (size_t i = number->Size; i > 0; i--)
        fprintf(file, "%c", '0' + GetDigit(number, i - 1));
}
