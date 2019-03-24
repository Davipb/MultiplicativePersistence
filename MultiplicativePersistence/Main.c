#include "Number.h"

#include <stdlib.h>
#include <inttypes.h>

void PrintResult(FILE* file, uintmax_t steps, LargeNumber* number)
{
    fprintf_s(file, "%"PRIuMAX" steps: ", steps);
    FPrintNumber(file, number);
    fprintf_s(file, "\n");
}

void main(int argc, char** argv)
{
    if (argc < 4)
    {
        printf("Usage: %s [starting digits] [max digits] [threshold]\n", argv[0]);
        printf("\tstarting digits: The number of digits where the searching will start\n");
        printf("\tmax digits: The number of digits where the searching will stop\n");
        printf("\tthreshold: The minimum number of steps a number has to hit for it to be reported as a result\n");
        return;
    }

    uintmax_t start;
    sscanf_s(argv[1], "%"PRIuMAX, &start);

    uintmax_t end;
    sscanf_s(argv[2], "%"PRIuMAX, &end);

    uintmax_t threshold;
    sscanf_s(argv[3], "%"PRIuMAX, &threshold);

    printf("Starting at %"PRIuMAX" digits\n", start);
    printf("Ending at %"PRIuMAX" digits\n", end);
    printf("With a minimum of %"PRIuMAX" steps\n", threshold);
    
    LargeNumber* current = SmallestWithDigits(start);
    while (NumberOfDigits(current) <= end)
    {
        size_t oldDigits = NumberOfDigits(current);
        Increment(current);
        size_t currentDigits = NumberOfDigits(current);

        if (currentDigits != oldDigits)
            printf("Checking numbers with %zu digits\n", currentDigits);
        
        if (!IsInSearchSpace(current))
            continue;

        uintmax_t steps = 0;
        LargeNumber* acc = CopyNumber(current);

        while (NumberOfDigits(acc) > 1)
        {
            steps++;
            LargeNumber* newAcc = MultiplyDigits(acc);
            FreeNumber(acc);
            acc = newAcc;
        }

        FreeNumber(acc);

        if (steps >= threshold)
        {
            PrintResult(stdout, steps, current);

            FILE* resultsFile;
            fopen_s(&resultsFile, "results.txt", "a");
            PrintResult(resultsFile, steps, current);
            fclose(resultsFile);
        }
    }

    FreeNumber(current);

    printf("Finished.\n");
}
