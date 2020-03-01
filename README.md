FileSort.c

NAME
filesort.c - c file
a.out - compiled binary executable

DESCRIPTION

The filesort program creates a binary executable that takes in a flag and a text file consisting of either strings or integers separated by commas and sorts the file with either a quicksort or insertion sort algorithm specified by the flag. White space in the text file is automatically removed (this includes spaces, tabs, and newlines). Function does all sorting through linked lists and tokenizing through the use of linked lists and prints out the final sorted linked list of tokens. Function also frees all use of heap memory prior to termination. 

USE

gcc filesort.c
./a.out -f inputfile.txt

-f - flag that indicates which sorting algorithm to use. Must be either "-q" or "-i" indicating quicksort or insertion sort respectively

inputfile.txt - input file containing either the strings or the integers to be sorted (cannot consist of both strings and integers). Each token must be comma-separated. 

ERRORS

Fatal Errors: These are errors that 