#include "Number.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <signal.h>

#define FAIL(...) { fprintf(stderr, __VA_ARGS__); exit(EXIT_FAILURE); }

// If the system has requested that the program be stopped
static volatile bool StopRequested = false;

// Handles a signal received from the system
// signal: The signal that was received
void SignalHandler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM)
        StopRequested = true;
}

// Reports that a result has been found
static void ReportResult(size_t steps, LargeNumber* number)
{
    printf("%zu steps: ", steps);
    FPrintNumber(stdout, number);
    printf("\n");

    int len = snprintf(NULL, 0, "result.%zu.txt", steps);
    char* fileName = calloc(len + 1, sizeof(char));
    snprintf(fileName, len + 1, "result.%zu.txt", steps);

    FILE* file = fopen(fileName, "a");
    if (file == NULL)
    {
        fprintf(stderr, "Unable to open %s to report result", fileName);
        return;
    }

    free(fileName);

    FPrintNumber(file, number);
    fprintf(file, "\n");

    fclose(file);
}

// Formats and prints the duration between two points in time to stdout
static void PrintDiff(time_t start, time_t end)
{
    double seconds = difftime(end, start);
    printf("%dd %dh %dmin %ds", (int)(seconds / 86400), (int)(seconds / 3600), (int)(seconds / 60) % 60, (int)seconds % 60);
}

// Prints time statistics to stdout
// programStart: When the program was started
// deltaStart: When the last unit of work was started
static void PrintTimeStats(time_t programStart, time_t deltaStart)
{
    time_t now = time(NULL);

    printf("Delta time: ");
    PrintDiff(deltaStart, now);
    printf("\n");

    printf("Total time: ");
    PrintDiff(programStart, now);
    printf("\n");
}

// Reads the program configuration its command-line arguments
static void ReadArgConfig(int argc, char** argv, uintmax_t* threshold, LargeNumber** start, LargeNumber** end)
{
    if (sscanf(argv[1], "%zu", threshold) < 1) 
        FAIL("Invalid threshold %s", argv[1]);
    
    *start = NULL;
    if (argc >= 3)
    {
        *start = SScanNumber(argv[2]);
        if (*start == NULL) FAIL("Invalid start number %s", argv[2]);
    }

    *end = NULL;
    if (argc >= 4)
    {
        *end = SScanNumber(argv[3]);
        if (*end == NULL) FAIL("Invalid end number %s", argv[3]);   
    }
}

// Reads the program configuration from files
static void ReadFileConfig(uintmax_t* threshold, LargeNumber** start, LargeNumber** end)
{
    FILE* thresholdFile = fopen("threshold.txt", "r");
    if (thresholdFile == NULL) FAIL("Unable to open threshold.txt");

    if (fscanf(thresholdFile, "%zu", threshold) < 1) FAIL("Invalid threshold number in threshold.txt");
    fclose(thresholdFile);

    *start = NULL;
    FILE* startFile = fopen("start.txt", "r");
    if (startFile != NULL)
    {
        *start = FScanNumber(startFile);
        if (*start == NULL) FAIL("Invalid start number in start.txt");

        fclose(startFile);
    }

    *end = NULL;
    FILE* endFile = fopen("end.txt", "r");
    if (endFile != NULL)
    {
        *end = FScanNumber(endFile);
        if (*end == NULL) FAIL("Invalid end number in end.txt");

        fclose(endFile);
    }
}

int main(int argc, char** argv)
{
    // Disable stdout bufferring
    // This slows down write operations, but ensures that results and progress reports will
    // be written as soon as available to the ouput terminal or log file
    // The processing time needed to check our numbers will vastly overshadow any small speed loss
    // from the eventual stdout prints
    setbuf(stdout, NULL);

    signal(SIGTERM, &SignalHandler);
    signal(SIGINT, &SignalHandler);

    uintmax_t threshold;
    LargeNumber* start;
    LargeNumber* end;

    bool fromFile = argc <= 1;
    if (fromFile)
        ReadFileConfig(&threshold, &start, &end);
    else
        ReadArgConfig(argc, argv, &threshold, &start, &end);

    if (start == NULL) start = SmallestWithDigits(1);

    printf("Starting at ");
    FPrintNumber(stdout, start);
    printf("\n");

    if (end != NULL)
    {
        printf("Ending at ");
        FPrintNumber(stdout, end);
        printf("\n");
    }

    printf("With a minimum of %"PRIuMAX" steps\n", threshold);    
    
    time_t programStart = time(NULL);
    uintmax_t numbersFound = 0;

    LargeNumber* current = start;
    time_t digitsStart = programStart;
    bool reportDigits = true;
    
    while (!StopRequested && (end == NULL || Compare(current, end) <= 0))
    {
        if (reportDigits)
        {
            printf("\n");
            printf("Now at %zu digits\n", NumberOfDigits(current));
            PrintTimeStats(programStart, digitsStart);

            digitsStart = time(NULL);
        }

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
        {
            numbersFound++;
            ReportResult(steps, current);
        }

        reportDigits = Increment(current);
    }

    if (fromFile)
    {
        FILE* file = fopen("start.txt", "w");
        if (file == NULL)
        {
            fprintf(stderr, "Unable to open start.txt");
        }
        else
        {
            FPrintNumber(file, current);
            fclose(file);
        }
    }

    FreeNumber(current);
    FreeNumber(end);

    printf("\n");
    printf("Finished\n");
    printf("Found %"PRIuMAX" results\n", numbersFound);
    PrintTimeStats(programStart, digitsStart);
}
