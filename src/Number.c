#include "Number.h"

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

// Take care when changing the base below
// First, verify that the typedef of 'Digit' can hold (Base - 1)^2 + floor((Base - 1)^2 / Base)
// Then, take a look at the 'BASE DEPENDANT' functions below and adjust them to fit your new base
#define BASE 10

// A type that can hold a single digit of a large number
// Ensure that this type can hold at least (Base - 1)^2 + floor((Base - 1)^2 / Base)
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

// Sets a digit of a number, automatically resizing the number if it isn't
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

// Multiplies a large number by the value of a digit in-place
static void Multiply(LargeNumber* number, Digit digit)
{
    if (number->Size == 0)
        return;

    if (digit == 0)
    {
        ResizeNumber(number, 0);
        return;
    }

    if (digit == 1)
        return;

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

void FPrintNumber(FILE* file, LargeNumber* number)
{
    if (number->Size == 0)
    {
        fprintf(file, "0");
        return;
    }

    for (size_t i = number->Size; i > 0; i--)
    {
        char c;
        Digit digit = GetDigit(number, i - 1);

        // Use 0 - 10 then A - Z

        if (digit < 10)
            c = '0' + digit;
        else
            c = 'A' + (digit - 10);

        fprintf(file, "%c", c);
    }
}

/*
+====================================================================================+
|                              [!] BASE DEPENDANT [!]                                |
+------------------------------------------------------------------------------------+
| The functions below have been optimized for Base 10 and will need adjusting if the |
| program is to work in other bases.                                                 |
|                                                                                    |
| Credit goes to David A. Corneth for finding the properties below.                  |
+====================================================================================+
*/

LargeNumber* SmallestWithDigits(size_t digits)
{
    // Smallest number is always 26777777...

    LargeNumber* result = NewNumber(digits);

    for (size_t i = 0; i < digits; i++)
    {
        Digit digit;
        if (i == 0) digit = 2;
        else if (i == 1) digit = 6;
        else digit = 7;

        SetDigit(result, digits - i - 1, digit);
    }

    return result;
}

// A candidate record-holding multiplicative persistence number in base 10 must contain only
// 7s, 8s, or 9s in nondescending order
// Additionally, a number may also start with one of six prefixes

// The type of a prefix of a large number, identifying which digits it contains
typedef enum PrefixType
{
    PREFIX_26,
    PREFIX_2,
    PREFIX_35,
    PREFIX_3,
    PREFIX_4,
    PREFIX_5s, // Variable-length: Any amount of 5s at the start of a number is considered a valid prefix
    PREFIX_6,
    PREFIX_NONE
} PrefixType;

// Represents the prefix of a large candidate number
typedef struct NumberPrefix
{
    // The length of this prefix, in digits, from the start of the number
    size_t Length;

    // The type of this prefix
    PrefixType Type;

} NumberPrefix;

// Finds the prefix of a large candidate number
static NumberPrefix FindPrefix(LargeNumber* number)
{
    NumberPrefix prefix = { 0, PREFIX_NONE };
    if (number->Size == 0) return prefix;

    Digit first = GetDigit(number, number->Size - 1);
    Digit second = 0;
    if (number->Size >= 2) second = GetDigit(number, number->Size - 2);

    if (first == 2)
    {
        if (second == 6)
        {
            prefix.Length = 2;
            prefix.Type = PREFIX_26;
            return prefix;
        }

        prefix.Length = 1;
        prefix.Type = PREFIX_2;
        return prefix;
    }

    if (first == 3)
    {
        if (second == 5)
        {
            prefix.Length = 2;
            prefix.Type = PREFIX_35;
            return prefix;
        }

        prefix.Length = 1;
        prefix.Type = PREFIX_3;
        return prefix;
    }

    if (first == 4)
    {
        prefix.Length = 1;
        prefix.Type = PREFIX_4;
        return prefix;
    }

    if (first == 5)
    {
        size_t i = number->Size - 1;
        while (GetDigit(number, i) == 5)
        {
            prefix.Length++;
            if (i == 0) break;
            i--;
        }

        prefix.Type = PREFIX_5s;
        return prefix;
    }

    if (first == 6)
    {
        prefix.Length = 1;
        prefix.Type = PREFIX_6;
        return prefix;
    }

    return prefix;
}

// Tries to increment the "regular" aka "non-prefix" part of a number, consisting of 7s, 8s, and 9s in
// nondecreasing order
// If the incrementing failed, that portion will be reset to all 7s and the function will return false.
// number: The number to be incremented
// start: The index of the first digit of the "regular" part of the number
static bool TryIncrementRegular(LargeNumber* number, size_t start)
{
    if (start >= number->Size) return false;

    if (GetDigit(number, start) == 9)
    {
        for (size_t i = 0; i <= start; i++)
            SetDigit(number, i, 7);

        return false;
    }

    // Find the first number before a 9
    size_t borderIndex = start;
    while (borderIndex > 0 && GetDigit(number, borderIndex - 1) != 9)
        borderIndex--;

    Digit border = GetDigit(number, borderIndex);
    SetDigit(number, borderIndex, border + 1);

    if (border == 7 && borderIndex > 0)
    {
        for (size_t i = 0; i < borderIndex; i++)
            SetDigit(number, i, 8);
    }

    return true;
}

bool Increment(LargeNumber* number)
{
    NumberPrefix prefix = FindPrefix(number);

    // We don't need to change the prefix if the regular part of the number can still be increased
    if (TryIncrementRegular(number, number->Size - prefix.Length - 1))
        return false;

    // The regular part of the number has overflowed and can no longer increase
    // Move to the next bigger prefix

    switch (prefix.Type)
    {
    case PREFIX_26: // 2699... -> 2777...
        SetDigit(number, number->Size - 2, 7); 
        return false;

    case PREFIX_2: // 2999... -> 3577...
        SetDigit(number, number->Size - 1, 3);
        if (number->Size >= 2) SetDigit(number, number->Size - 2, 5);
        return false;

    case PREFIX_35: // 3599... -> 3777...
        SetDigit(number, number->Size - 2, 7);
        return false;

    case PREFIX_3: // 3999... -> 4777...
        SetDigit(number, number->Size - 1, 4);
        return false;

    case PREFIX_4: // 4999... -> 5555...
        for (size_t i = 0; i < number->Size; i++)
            SetDigit(number, i, 5);
        return false;

    case PREFIX_5s:
        if (prefix.Length == 1) // 5999... -> 6777...            
            SetDigit(number, number->Size - 1, 6);

        else // 5559... -> 5577...            
            SetDigit(number, number->Size - prefix.Length, 7);

        return false;

    case PREFIX_6: // 6999... -> 7777...
        SetDigit(number, number->Size - 1, 7);
        return false;

    case PREFIX_NONE: // 9999... -> 26777...
        if (number->Size >= 1) SetDigit(number, number->Size - 1, 6);
        SetDigit(number, number->Size, 2);
        return true;

    default:
        fprintf(stderr, "FATAL ERROR");
        exit(EXIT_FAILURE);
    }
}
