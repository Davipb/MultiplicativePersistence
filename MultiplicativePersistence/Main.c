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

// Reads a numeric value from a string, automatically exiting on error
static size_t ReadConfig(char* buffer)
{
    size_t result;
    if (sscanf_s(buffer, "%zu", &result) != 1)
    {
        fprintf(stderr, "Invalid number '%s'", buffer);
        exit(EXIT_FAILURE);
    }

    return result;
}

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        printf("Usage: %s [start] [end] [threshold]\n", argv[0]);
        printf("\tstart: Number of digits to start searching at\n");
        printf("\tend: Number of digits to stop searching at\n");
        printf("\tthreshold: The minimum number of steps that will be considered a result\n");
        return EXIT_FAILURE;
    }

    size_t start = ReadConfig(argv[1]);
    size_t end = ReadConfig(argv[2]);
    size_t threshold = ReadConfig(argv[3]);
    
    printf("Starting at %zu digits\n", start);
    printf("Ending at %zu digits\n", end);
    printf("With a minimum of %zu steps\n", threshold);
    
    LargeNumber* current = SmallestWithDigits(start);
    bool reportDigits = true;

    while (NumberOfDigits(current) <= end)
    {
        if (reportDigits)
            printf("Now at %zu digits\n", NumberOfDigits(current));

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

        reportDigits = Increment(current);
    }

    FreeNumber(current);

    printf("Finished.\n");
}
