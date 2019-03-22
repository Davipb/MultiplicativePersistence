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
    size_t newSize = number->Size;

    while (number->Digits[newSize - 1] == 0)
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

LargeNumber* MakeLarge(uintmax_t value)
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

// Adds two large numbers
LargeNumber* Add(LargeNumber* a, LargeNumber* b)
{
    LargeNumber* sum = NewNumber(max(a->Size, b->Size));

    Digit carry = 0;
    for (size_t i = 0; i < sum->Size; i++)
    {
        Digit value = GetDigit(a, i) + GetDigit(b, i) + carry;
        carry = value / BASE;
        SetDigit(sum, i, value %= BASE);
    }

    if (carry != 0)
        SetDigit(sum, sum->Size, carry);

    TrimNumber(sum);
    return sum;
}

LargeNumber* Increment(LargeNumber* number)
{
    LargeNumber* one = MakeLarge(1);
    LargeNumber* result = Add(number, one);
    FreeNumber(one);
    return result;
}

// Multiplies two large numbers
LargeNumber* Multiply(LargeNumber* a, LargeNumber* b)
{
    // If either number has size 0, return 0
    if (a->Size == 0 || b->Size == 0)
        return NewNumber(0);

    LargeNumber* result = NewNumber(max(a->Size, b->Size));

    for (size_t bIndex = 0; bIndex < b->Size; bIndex++)
    {
        Digit multiplicand = GetDigit(b, bIndex);

        Digit carry = 0;
        for (size_t aIndex = 0; aIndex < a->Size; aIndex++)
        {
            size_t resultIndex = bIndex + aIndex;

            Digit value = GetDigit(a, aIndex) * multiplicand + GetDigit(result, resultIndex) + carry;
            carry = value / BASE;
            SetDigit(result, resultIndex, value % BASE);
        }

        if (carry != 0)
            SetDigit(result, bIndex + a->Size, carry);
    }

    TrimNumber(result);
    return result;
}

LargeNumber* MultiplyDigits(LargeNumber* number)
{
    if (number->Size == 0)
        return MakeLarge(0);

    if (number->Size == 1)
        return MakeLarge(GetDigit(number, 0));

    LargeNumber* acc = MakeLarge(1);

    for (size_t i = 0; i < number->Size; i++)
    {
        LargeNumber* digit = MakeLarge(GetDigit(number, i));
        LargeNumber* newAcc = Multiply(acc, digit);

        FreeNumber(digit);
        FreeNumber(acc);

        acc = newAcc;
    }

    TrimNumber(acc);
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