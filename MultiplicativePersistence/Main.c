#include "Number.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

// Reports that a result has been found
static void ReportResult(size_t steps, LargeNumber* number)
{
    printf("%zu steps: ", steps);
    FPrintNumber(stdout, number);
    printf("\n");

    int len = snprintf(NULL, 0, "result.%zu.txt", steps);
    char* fileName = calloc(len + 1, sizeof(char));
    snprintf(fileName, len + 1, "result.%zu.txt", steps);

    FILE* file;
    if (fopen_s(&file, fileName, "a") != 0)
    {
        fprintf(stderr, "Unable to open %s to report result", fileName);
        return;
    }

    free(fileName);

    FPrintNumber(file, number);
    fprintf(file, "\n");

    fclose(file);
}

// Reads a value from a configuration file
static size_t ReadConfig(const char* fileName)
{
    FILE* file;
    if (fopen_s(&file, fileName, "r") != 0)
    {
        fprintf(stderr, "Unable to open %s", fileName);
        exit(EXIT_FAILURE);
    }

    size_t result;
    if (fscanf_s(file, "%zu", &result) < 1)
    {
        fprintf(stderr, "Unable to read an integer from %s", fileName);
        exit(EXIT_FAILURE);
    }

    fclose(file);
    return result;
}

void main()
{
    size_t start = ReadConfig("start.txt");
    size_t end = ReadConfig("end.txt");
    size_t threshold = ReadConfig("threshold.txt");
    
    printf("Starting at %zu digits\n", start);
    printf("Ending at %zu digits\n", end);
    printf("With a minimum of %zu steps\n", threshold);
    
    LargeNumber* current = SmallestWithDigits(start);
    while (NumberOfDigits(current) <= end)
    {
        size_t steps = 0;
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
            ReportResult(steps, current);

        size_t oldDigits = NumberOfDigits(current);
        Increment(current);
        size_t currentDigits = NumberOfDigits(current);

        if (currentDigits != oldDigits)
            printf("Now at %zu digits\n", currentDigits);
    }

    FreeNumber(current);

    printf("Finished.\n");
}
