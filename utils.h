#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/* ANSI Terminal Color Codes */
#define COLOR_RESET       "\033[0m"
#define COLOR_BOLD        "\033[1m"
#define COLOR_DIM         "\033[2m"
#define COLOR_RED         "\033[1;31m"
#define COLOR_GREEN       "\033[1;32m"
#define COLOR_YELLOW      "\033[1;33m"
#define COLOR_BLUE        "\033[1;34m"
#define COLOR_MAGENTA     "\033[1;35m"
#define COLOR_CYAN        "\033[1;36m"
#define COLOR_WHITE       "\033[1;37m"
#define COLOR_GRAY        "\033[90m"

/* Backgrounds */
#define BG_CYAN           "\033[46;30m"
#define BG_DARK           "\033[40m"

/* Function Prototypes */

/**
 * Initializes console environment (enables ANSI escape sequences on Windows).
 */
void initConsole(void);

/**
 * Clears the terminal screen portably.
 */
void clearScreen(void);

/**
 * Pauses execution until the user presses Enter.
 */
void pausePrompt(void);

/**
 * Trims leading and trailing whitespace characters in-place.
 */
void trimWhitespace(char *str);

/**
 * Safely reads a line of text from standard input, removing trailing newline.
 * Prevents buffer overflows.
 *
 * @param dest   Destination buffer
 * @param maxLen Maximum number of characters to store (including null terminator)
 */
void readString(char *dest, int maxLen);

/**
 * Reads an integer safely from standard input.
 * Returns 1 on success, 0 on invalid input.
 */
int readInt(int *outVal);

/**
 * Reads an integer within a specified [minVal, maxVal] range.
 * Loops until a valid input is entered or returns 0 if cancelled.
 */
int readIntRange(int *outVal, int minVal, int maxVal);

/**
 * Determines whether a given year is a leap year.
 */
int isLeapYear(int year);

/**
 * Validates whether the given year, month, and day form a valid calendar date.
 */
int isValidCalendarDate(int year, int month, int day);

/**
 * Validates a date string in YYYY-MM-DD format with complete calendar checking.
 * Returns 1 if valid, 0 otherwise.
 */
int validateDate(const char *dateStr);

/**
 * Retrieves the current system date as a string in YYYY-MM-DD format.
 */
void getCurrentDateString(char *dest, int maxLen);

/**
 * Compares two date strings in YYYY-MM-DD format.
 * Returns:
 *   < 0 if date1 < date2
 *     0 if date1 == date2
 *   > 0 if date1 > date2
 */
int compareDates(const char *date1, const char *date2);

/**
 * Case-insensitive substring search.
 * Returns 1 if needle is found inside haystack, 0 otherwise.
 */
int strCaseStr(const char *haystack, const char *needle);

/**
 * Prints a repeated character for dividers.
 */
void printDivider(char ch, int length);

#endif /* UTILS_H */

