#include "utils.h"

void initConsole(void) {
    /* Invoking system("") triggers Windows console to enable ANSI VT100 mode */
    system("");
}

void clearScreen(void) {
    /* Send ANSI clear screen and home cursor */
    printf("\033[2J\033[H");
    fflush(stdout);
}

void pausePrompt(void) {
    printf("\n" COLOR_GRAY "Press [Enter] to continue..." COLOR_RESET);
    fflush(stdout);
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* consume any pending character */
    }
}

void trimWhitespace(char *str) {
    if (str == NULL) return;

    /* Trim leading whitespace */
    char *start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    /* Trim trailing whitespace */
    int len = (int)strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[len - 1] = '\0';
        len--;
    }
}

void readString(char *dest, int maxLen) {
    if (dest == NULL || maxLen <= 0) return;

    if (fgets(dest, maxLen, stdin) != NULL) {
        size_t len = strlen(dest);
        if (len > 0 && dest[len - 1] == '\n') {
            dest[len - 1] = '\0';
        } else {
            /* Input exceeded maxLen - clear remainder of standard input stream */
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        }
    } else {
        dest[0] = '\0';
    }
}

int readInt(int *outVal) {
    char buffer[64];
    readString(buffer, sizeof(buffer));
    trimWhitespace(buffer);

    if (strlen(buffer) == 0) {
        return 0;
    }

    char extra;
    if (sscanf(buffer, "%d %c", outVal, &extra) == 1) {
        return 1;
    }
    return 0;
}

int readIntRange(int *outVal, int minVal, int maxVal) {
    int val;
    if (readInt(&val)) {
        if (val >= minVal && val <= maxVal) {
            *outVal = val;
            return 1;
        }
    }
    return 0;
}

int isLeapYear(int year) {
    return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
}

int isValidCalendarDate(int year, int month, int day) {
    if (year < 2000 || year > 2100) {
        return 0;
    }
    if (month < 1 || month > 12) {
        return 0;
    }
    if (day < 1) {
        return 0;
    }

    int maxDays = 31;
    switch (month) {
        case 4:
        case 6:
        case 9:
        case 11:
            maxDays = 30;
            break;
        case 2:
            maxDays = isLeapYear(year) ? 29 : 28;
            break;
        default:
            maxDays = 31;
            break;
    }

    return (day <= maxDays);
}

int validateDate(const char *dateStr) {
    if (dateStr == NULL) return 0;
    if (strlen(dateStr) != 10) return 0;

    /* Verify YYYY-MM-DD format structure */
    if (dateStr[4] != '-' || dateStr[7] != '-') return 0;

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)dateStr[i])) return 0;
    }

    int year = 0, month = 0, day = 0;
    if (sscanf(dateStr, "%4d-%2d-%2d", &year, &month, &day) != 3) {
        return 0;
    }

    return isValidCalendarDate(year, month, day);
}

void getCurrentDateString(char *dest, int maxLen) {
    if (dest == NULL || maxLen < 11) return;

    time_t now = time(NULL);
    struct tm *local = localtime(&now);

    if (local != NULL) {
        snprintf(dest, maxLen, "%04d-%02d-%02d",
                 local->tm_year + 1900,
                 local->tm_mon + 1,
                 local->tm_mday);
    } else {
        strncpy(dest, "2026-01-01", maxLen);
        dest[maxLen - 1] = '\0';
    }
}

int compareDates(const char *date1, const char *date2) {
    if (date1 == NULL && date2 == NULL) return 0;
    if (date1 == NULL) return -1;
    if (date2 == NULL) return 1;
    return strcmp(date1, date2);
}

int strCaseStr(const char *haystack, const char *needle) {
    if (haystack == NULL || needle == NULL) return 0;
    if (*needle == '\0') return 1;

    size_t needleLen = strlen(needle);
    size_t haystackLen = strlen(haystack);

    if (needleLen > haystackLen) return 0;

    for (size_t i = 0; i <= haystackLen - needleLen; i++) {
        size_t j = 0;
        while (j < needleLen) {
            char ch1 = (char)tolower((unsigned char)haystack[i + j]);
            char ch2 = (char)tolower((unsigned char)needle[j]);
            if (ch1 != ch2) break;
            j++;
        }
        if (j == needleLen) {
            return 1;
        }
    }
    return 0;
}

void printDivider(char ch, int length) {
    for (int i = 0; i < length; i++) {
        putchar(ch);
    }
    putchar('\n');
}

