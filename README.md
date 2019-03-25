# Multiplicative Persistence Finder
A program that can find record-holding Multiplicative Persistence numbers. 
Take a number, multiply its digits, repeat until you hit a 1-digit number. 
The amount of steps it took to reach the 1-digit number is the Multiplicative Persistence of that number. 

For example, consider the number 679:
1. 6 * 7 * 9 = 378
2. 3 * 7 * 8 = 168
3. 1 * 6 * 8 = 48
4. 4 * 8 = 32
5. 3 * 2 = 6

It took 5 steps to reach a 1-digit number (in this case, 6), so the multiplicative persistence of 679 is 5.
In fact, 679 is the smallest number with a multiplicative persistence of 5, and so it is considered a record-holder.

For more information, please check the [Numberphile video that insipried this program](https://www.youtube.com/watch?v=Wim9WJeDTHQ) and the [OEIS sequence for record-holding multiplicative persistence numbers](https://oeis.org/A003001).

## Usage
The program has three startup values:
* `threshold`: The minimum amount of digit multiplication steps a number must hit to be considered a result
* `start` (Optional): The number to start searching at. If not set, defaults to the smallest number with 1 digit
* `end` (Optional): The number to stop searching at. If not set, the program will never stop on its own. 
If the program is executed without any arguments, it reads the startup values from files in the current working directory: `threshold.txt`, `start.txt`, and `end.txt`.
If executed with arguments, it reads the startup values from the arguments, in the order `threshold` `start` `end`.

For `start` and `end`, you can prepend `@` to the number to specify an amount of digits instead of an actual number (both in file mode and in argument mode).
For example, to search for numbers with at least 5 steps between 100 and 200 digits, use the arguments `5 @100 @200`.

Once a result is found (a number with a number of steps higher or equal to `threshold`), the number is appended as a new line to a file named `result.(steps).txt` on the working directory, where `(steps)` is the number of steps of that number.

The search is not "dumb": It uses known properties of record-holding numbers discovered by David A. Corneth to significantly reduce the amount of numbers that have to be checked.

## Adapting for other bases
Only the search space algorithm based on David A. Corneth's discoveries is dependant on the base of the numbers, the rest of the program is completely base-agnostic.
For more information on how to adapt the program for other bases, check the base definition at the top of `Number.c`.
