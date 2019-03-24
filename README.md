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
The program expects to find three files in its working directory: `start.txt`, `end.txt`, and `threshold.txt`, all containing a single integer number. `start.txt` must have the number of digits to start the search at, `end.txt` must have the number of digits to end the search at, and `threshold.txt` must have the smallest number of steps that a number has to reach to be reported as a result.

Once a result is found (a number with a number of steps higher or equal to the `threshold.txt`), the number is appended as a new line to a file named `result.(steps).txt` on the working directory, where `(steps)` is the number of steps of that number.

The search is not "dumb": It uses known properties of record-holding numbers discovered by David A. Corneth to significantly reduce the amount of numbers that have to be checked.

## Adapting for other bases
Only the search space algorithm based on David A. Corneth's discoveries is dependant on the base of the numbers, the rest of the program is completely base-agnostic.
For more information on how to adapt the program for other bases, check the base definition at the top of `Number.c`.
