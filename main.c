#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "utils.h"

/* ========================================================================= */
/*                          DASHBOARD & UI RENDERING                         */
/* ========================================================================= */

static void renderBanner(const char *currentDate) {
    printf(COLOR_CYAN);
    printDivider('=', 80);
    printf("  " COLOR_BOLD "TASKZERO v1.0" COLOR_RESET COLOR_CYAN
           "  |  " COLOR_WHITE "PERSONAL PRODUCTIVITY SYSTEM" COLOR_RESET COLOR_CYAN
           "  |  Date: %s\n", currentDate ? currentDate : "N/A");
    printDivider('=', 80);
    printf(COLOR_RESET);
}

static void renderProgressBar(double percentage, int barWidth) {
    if (barWidth <= 0) barWidth = 20;
    if (percentage < 0.0) percentage = 0.0;
    if (percentage > 100.0) percentage = 100.0;

    int filled = (int)((percentage / 100.0) * barWidth);
    if (filled > barWidth) filled = barWidth;

    printf("[");
    if (percentage >= 75.0) {
        printf(COLOR_GREEN);
    } else if (percentage >= 40.0) {
        printf(COLOR_CYAN);
    } else if (percentage > 0.0) {
        printf(COLOR_YELLOW);
    } else {
        printf(COLOR_GRAY);
    }

    for (int i = 0; i < filled; i++) {
        putchar('#');
    }
    printf(COLOR_GRAY);
    for (int i = filled; i < barWidth; i++) {
        putchar('-');
    }
    printf(COLOR_RESET "] %5.1f%%", percentage);
}

static void renderDashboard(const Task tasks[], int count, const char *currentDate) {
    int total = 0, completed = 0, pending = 0, overdue = 0;
    int low = 0, med = 0, high = 0;

    getTaskStatistics(tasks, count, currentDate,
                      &total, &completed, &pending, &overdue,
                      &low, &med, &high);

    double score = calculateProductivityScore(completed, total);

    printf("\n" COLOR_CYAN "+------------------------------------------------------------------------------+\n");
    printf("|" COLOR_BOLD COLOR_WHITE "                            PRODUCTIVITY DASHBOARD                            " COLOR_RESET COLOR_CYAN "|\n");
    printf("+------------------------------------------------------------------------------+\n");

    /* Row 1 */
    printf("|  " COLOR_WHITE "TOTAL TASKS:     " COLOR_BOLD "%-4d" COLOR_RESET COLOR_CYAN "       |  "
           COLOR_WHITE "PRODUCTIVITY SCORE: " COLOR_RESET, total);
    renderProgressBar(score, 18);
    printf(COLOR_CYAN "  |\n");

    /* Row 2 */
    printf("|  " COLOR_GREEN "COMPLETED TASKS: " COLOR_BOLD "%-4d" COLOR_RESET COLOR_CYAN "       |  "
           COLOR_GRAY "Formula: (Completed / Total) * 100" COLOR_RESET COLOR_CYAN "         |\n", completed);

    /* Row 3 */
    printf("|  " COLOR_YELLOW "PENDING TASKS:   " COLOR_BOLD "%-4d" COLOR_RESET COLOR_CYAN "       |  "
           COLOR_WHITE "PRIORITY OVERVIEW:                              " COLOR_RESET COLOR_CYAN "|\n", pending);

    /* Row 4 */
    printf("|  " COLOR_RED "OVERDUE TASKS:   " COLOR_BOLD "%-4d" COLOR_RESET COLOR_CYAN "       |  "
           COLOR_RED "High: %-2d" COLOR_RESET COLOR_CYAN "  |  "
           COLOR_YELLOW "Medium: %-2d" COLOR_RESET COLOR_CYAN "  |  "
           COLOR_BLUE "Low: %-2d" COLOR_RESET COLOR_CYAN "          |\n",
           overdue, high, med, low);

    printf("+------------------------------------------------------------------------------+\n" COLOR_RESET);
}

static void renderTaskTable(const Task tasks[], const int indices[], int count,
                            const char *tableTitle, const char *currentDate) {
    if (tableTitle != NULL) {
        printf("\n" COLOR_BOLD COLOR_CYAN ">>> %s (%d task%s)" COLOR_RESET "\n",
               tableTitle, count, (count == 1) ? "" : "s");
    }

    printf(COLOR_CYAN "+------+----------------------------------+----------+------------+------------+-----------+\n");
    printf("| " COLOR_BOLD " ID " COLOR_RESET COLOR_CYAN " | "
           COLOR_BOLD "Title                            " COLOR_RESET COLOR_CYAN " | "
           COLOR_BOLD "Priority" COLOR_RESET COLOR_CYAN " | "
           COLOR_BOLD " Due Date  " COLOR_RESET COLOR_CYAN " | "
           COLOR_BOLD "  Status  " COLOR_RESET COLOR_CYAN " | "
           COLOR_BOLD "  Notes  " COLOR_RESET COLOR_CYAN " |\n");
    printf("+------+----------------------------------+----------+------------+------------+-----------+\n" COLOR_RESET);

    if (count == 0) {
        printf("|" COLOR_GRAY "                    No tasks found matching your request.                     " COLOR_RESET "|\n");
        printf(COLOR_CYAN "+------+----------------------------------+----------+------------+------------+-----------+\n" COLOR_RESET);
        return;
    }

    for (int k = 0; k < count; k++) {
        int i = indices ? indices[k] : k;
        const Task *t = &tasks[i];

        char displayTitle[33];
        size_t titleLen = strlen(t->title);
        if (titleLen > 32) {
            strncpy(displayTitle, t->title, 29);
            displayTitle[29] = '.';
            displayTitle[30] = '.';
            displayTitle[31] = '.';
            displayTitle[32] = '\0';
        } else {
            strncpy(displayTitle, t->title, sizeof(displayTitle));
            displayTitle[sizeof(displayTitle) - 1] = '\0';
        }

        int overdue = isTaskOverdue(t, currentDate);
        const char *statusName = getStatusName(t->status);
        const char *statusColor = getStatusColor(t->status);
        const char *priorityName = getPriorityName(t->priority);
        const char *priorityColor = getPriorityColor(t->priority);

        printf("| %4d | %-32.32s | %s%-8s" COLOR_RESET " | %-10s | %s%-10s" COLOR_RESET " | ",
               t->id,
               displayTitle,
               priorityColor, priorityName,
               t->dueDate,
               statusColor, statusName);

        if (overdue) {
            printf(COLOR_RED "[OVERDUE] " COLOR_RESET "|\n");
        } else if (t->status == STATUS_COMPLETED) {
            printf(COLOR_GREEN "[DONE]    " COLOR_RESET "|\n");
        } else {
            printf(COLOR_GRAY "[PENDING] " COLOR_RESET "|\n");
        }
    }

    printf(COLOR_CYAN "+------+----------------------------------+----------+------------+------------+-----------+\n" COLOR_RESET);
}

static void renderAllTasksTable(const Task tasks[], int count, const char *currentDate) {
    renderTaskTable(tasks, NULL, count, "ALL REGISTERED TASKS", currentDate);
}

static void renderTaskDetailCard(const Task *task, const char *currentDate) {
    if (task == NULL) return;

    int overdue = isTaskOverdue(task, currentDate);

    printf("\n" COLOR_CYAN "+------------------------------------------------------------------------------+\n");
    printf("|" COLOR_BOLD COLOR_WHITE " TASK DETAILS - ID #%03d                                                       " COLOR_RESET COLOR_CYAN "|\n", task->id);
    printf("+------------------------------------------------------------------------------+\n" COLOR_RESET);
    printf("  " COLOR_BOLD "ID:          " COLOR_RESET "#%d\n", task->id);
    printf("  " COLOR_BOLD "Title:       " COLOR_RESET "%s\n", task->title);
    printf("  " COLOR_BOLD "Status:      " COLOR_RESET "%s%s" COLOR_RESET "%s\n",
           getStatusColor(task->status),
           getStatusName(task->status),
           overdue ? COLOR_RED " [OVERDUE - DEADLINE PASSED]" COLOR_RESET : "");
    printf("  " COLOR_BOLD "Priority:    " COLOR_RESET "%s%s" COLOR_RESET "\n",
           getPriorityColor(task->priority),
           getPriorityName(task->priority));
    printf("  " COLOR_BOLD "Due Date:    " COLOR_RESET "%s\n", task->dueDate);
    printf("  " COLOR_BOLD "Description: " COLOR_RESET "%s\n", task->description);
    printf(COLOR_CYAN "+------------------------------------------------------------------------------+\n" COLOR_RESET);
}

static void renderMainMenu(void) {
    printf("\n" COLOR_CYAN "+------------------------------------------------------------------------------+\n");
    printf("|" COLOR_BOLD COLOR_WHITE "                               MAIN NAVIGATION                                " COLOR_RESET COLOR_CYAN "|\n");
    printf("+------------------------------------------------------------------------------+\n" COLOR_RESET);
    printf("  " COLOR_BOLD "[1]" COLOR_RESET " Add New Task                    "
           COLOR_BOLD "[4]" COLOR_RESET " Edit Task\n");
    printf("  " COLOR_BOLD "[2]" COLOR_RESET " View All Tasks                  "
           COLOR_BOLD "[5]" COLOR_RESET " Delete Task\n");
    printf("  " COLOR_BOLD "[3]" COLOR_RESET " Search Tasks                    "
           COLOR_BOLD "[6]" COLOR_RESET " Mark Task as Completed\n");
    printf("                                       "
           COLOR_BOLD "[0]" COLOR_RESET "  Save & Exit\n");
    printf(COLOR_CYAN "+------------------------------------------------------------------------------+\n" COLOR_RESET);
    printf("Enter choice [0-6]: ");
    fflush(stdout);
}

/* ========================================================================= */
/*                     INTERACTIVE FEATURE HANDLERS                          */
/* ========================================================================= */

static void handleAddTask(Task tasks[], int *taskCount, int *nextTaskId, const char *currentDate) {
    clearScreen();
    renderBanner(currentDate);
    printf("\n" COLOR_BOLD COLOR_CYAN "=== FEATURE 1: ADD NEW TASK ===" COLOR_RESET "\n\n");

    if (*taskCount >= MAX_TASKS) {
        printf(COLOR_RED "Error: Maximum capacity reached (%d tasks). Delete some tasks first." COLOR_RESET "\n", MAX_TASKS);
        pausePrompt();
        return;
    }

    char title[TITLE_LEN];
    char description[DESC_LEN];
    char dueDate[DATE_LEN];
    int priority = PRIORITY_MEDIUM;

    /* 1. Task Title (Mandatory, non-empty) */
    while (1) {
        printf("Enter Task Title (max %d chars, or 'cancel' to abort):\n> ", TITLE_LEN - 1);
        readString(title, sizeof(title));
        trimWhitespace(title);

        if (strlen(title) == 0) {
            printf(COLOR_YELLOW "Task title cannot be empty. Please enter a valid title." COLOR_RESET "\n\n");
            continue;
        }
        if (strcmp(title, "cancel") == 0) {
            printf(COLOR_GRAY "Task creation aborted." COLOR_RESET "\n");
            pausePrompt();
            return;
        }
        break;
    }

    /* 2. Task Description (Optional) */
    printf("\nEnter Task Description (max %d chars, press [Enter] to skip):\n> ", DESC_LEN - 1);
    readString(description, sizeof(description));
    trimWhitespace(description);
    if (strlen(description) == 0) {
        strncpy(description, "No description provided.", sizeof(description) - 1);
        description[sizeof(description) - 1] = '\0';
    }

    /* 3. Due Date (YYYY-MM-DD with full calendar validation) */
    while (1) {
        char dateBuf[64];
        printf("\nEnter Due Date (YYYY-MM-DD) [press Enter for today: %s]:\n> ", currentDate);
        readString(dateBuf, sizeof(dateBuf));
        trimWhitespace(dateBuf);

        if (strlen(dateBuf) == 0) {
            strncpy(dueDate, currentDate, sizeof(dueDate) - 1);
            dueDate[sizeof(dueDate) - 1] = '\0';
            break;
        }

        if (validateDate(dateBuf)) {
            strncpy(dueDate, dateBuf, sizeof(dueDate) - 1);
            dueDate[sizeof(dueDate) - 1] = '\0';
            break;
        } else {
            printf(COLOR_RED "Invalid date format or calendar date! Please use YYYY-MM-DD (e.g. 2026-11-20)." COLOR_RESET "\n");
        }
    }

    /* 4. Priority selection (1=LOW, 2=MEDIUM, 3=HIGH) */
    while (1) {
        char prioBuf[32];
        printf("\nSelect Priority (1 = LOW, 2 = MEDIUM, 3 = HIGH) [press Enter for default: 2 (MEDIUM)]:\n> ");
        readString(prioBuf, sizeof(prioBuf));
        trimWhitespace(prioBuf);

        if (strlen(prioBuf) == 0) {
            priority = PRIORITY_MEDIUM;
            break;
        }

        int pVal = 0;
        if (sscanf(prioBuf, "%d", &pVal) == 1 && pVal >= PRIORITY_LOW && pVal <= PRIORITY_HIGH) {
            priority = pVal;
            break;
        } else {
            printf(COLOR_RED "Invalid priority. Please enter 1 (LOW), 2 (MEDIUM), or 3 (HIGH)." COLOR_RESET "\n");
        }
    }

    /* Add task and auto-generate unique ID */
    int newId = addTask(tasks, taskCount, nextTaskId, title, description, dueDate, priority);

    if (newId != -1) {
        /* Permanent auto-save after every successful operation */
        saveTasks(tasks, *taskCount, *nextTaskId, DEFAULT_DATA_FILE);

        printf("\n" COLOR_GREEN "[SUCCESS] Task #%d created and saved to " DEFAULT_DATA_FILE " successfully!" COLOR_RESET "\n", newId);
        int idx = findTaskIndexById(tasks, *taskCount, newId);
        if (idx != -1) {
            renderTaskDetailCard(&tasks[idx], currentDate);
        }
    } else {
        printf("\n" COLOR_RED "[ERROR] Failed to add task." COLOR_RESET "\n");
    }

    pausePrompt();
}

static void handleViewAllTasks(const Task tasks[], int taskCount, const char *currentDate) {
    clearScreen();
    renderBanner(currentDate);
    renderAllTasksTable(tasks, taskCount, currentDate);
    pausePrompt();
}

static void handleSearchTasks(const Task tasks[], int taskCount, const char *currentDate) {
    clearScreen();
    renderBanner(currentDate);
    printf("\n" COLOR_BOLD COLOR_CYAN "=== FEATURE 3: SEARCH TASKS ===" COLOR_RESET "\n\n");

    if (taskCount == 0) {
        printf(COLOR_GRAY "No tasks in system to search." COLOR_RESET "\n");
        pausePrompt();
        return;
    }

    char keyword[100];
    printf("Enter search keyword or title phrase:\n> ");
    readString(keyword, sizeof(keyword));
    trimWhitespace(keyword);

    if (strlen(keyword) == 0) {
        printf(COLOR_YELLOW "Search term cannot be empty." COLOR_RESET "\n");
        pausePrompt();
        return;
    }

    int matchIndices[MAX_TASKS];
    int matchCount = searchTasks(tasks, taskCount, keyword, matchIndices);

    char titleBuf[128];
    snprintf(titleBuf, sizeof(titleBuf), "SEARCH RESULTS FOR: \"%s\"", keyword);
    renderTaskTable(tasks, matchIndices, matchCount, titleBuf, currentDate);

    pausePrompt();
}

static void handleEditTask(Task tasks[], int taskCount, int nextTaskId, const char *currentDate) {
    clearScreen();
    renderBanner(currentDate);
    printf("\n" COLOR_BOLD COLOR_CYAN "=== FEATURE 4: EDIT TASK ===" COLOR_RESET "\n\n");

    if (taskCount == 0) {
        printf(COLOR_GRAY "No tasks available to edit." COLOR_RESET "\n");
        pausePrompt();
        return;
    }

    printf("Enter Task ID to edit:\n> ");
    int id = 0;
    if (!readInt(&id)) {
        printf(COLOR_RED "Invalid ID entered." COLOR_RESET "\n");
        pausePrompt();
        return;
    }

    int idx = findTaskIndexById(tasks, taskCount, id);
    if (idx == -1) {
        printf(COLOR_RED "Error: Task with ID #%d was not found." COLOR_RESET "\n", id);
        pausePrompt();
        return;
    }

    printf("\n" COLOR_CYAN "Current Task Details:" COLOR_RESET "\n");
    renderTaskDetailCard(&tasks[idx], currentDate);

    printf("\n" COLOR_BOLD "Enter new values (leave blank and press [Enter] to keep current value):" COLOR_RESET "\n\n");

    /* New Title */
    char newTitle[TITLE_LEN];
    printf("New Title [Current: %s]:\n> ", tasks[idx].title);
    readString(newTitle, sizeof(newTitle));
    trimWhitespace(newTitle);

    /* New Description */
    char newDesc[DESC_LEN];
    printf("\nNew Description [Current: %s]:\n> ", tasks[idx].description);
    readString(newDesc, sizeof(newDesc));
    trimWhitespace(newDesc);

    /* New Due Date */
    char newDueDate[DATE_LEN];
    while (1) {
        char dateBuf[64];
        printf("\nNew Due Date [Current: %s] (YYYY-MM-DD):\n> ", tasks[idx].dueDate);
        readString(dateBuf, sizeof(dateBuf));
        trimWhitespace(dateBuf);

        if (strlen(dateBuf) == 0) {
            newDueDate[0] = '\0';
            break;
        }

        if (validateDate(dateBuf)) {
            strncpy(newDueDate, dateBuf, sizeof(newDueDate) - 1);
            newDueDate[sizeof(newDueDate) - 1] = '\0';
            break;
        } else {
            printf(COLOR_RED "Invalid date format or calendar date! Please use YYYY-MM-DD." COLOR_RESET "\n");
        }
    }

    /* Apply edits */
    editTask(tasks, taskCount, id,
             (strlen(newTitle) > 0) ? newTitle : NULL,
             (strlen(newDesc) > 0) ? newDesc : NULL,
             (strlen(newDueDate) > 0) ? newDueDate : NULL);

    saveTasks(tasks, taskCount, nextTaskId, DEFAULT_DATA_FILE);

    printf("\n" COLOR_GREEN "[SUCCESS] Task #%d successfully updated and saved!" COLOR_RESET "\n", id);
    renderTaskDetailCard(&tasks[idx], currentDate);

    pausePrompt();
}

static void handleDeleteTask(Task tasks[], int *taskCount, int nextTaskId, const char *currentDate) {
    clearScreen();
    renderBanner(currentDate);
    printf("\n" COLOR_BOLD COLOR_CYAN "=== FEATURE 5: DELETE TASK ===" COLOR_RESET "\n\n");

    if (*taskCount == 0) {
        printf(COLOR_GRAY "No tasks available to delete." COLOR_RESET "\n");
        pausePrompt();
        return;
    }

    printf("Enter Task ID to delete:\n> ");
    int id = 0;
    if (!readInt(&id)) {
        printf(COLOR_RED "Invalid ID entered." COLOR_RESET "\n");
        pausePrompt();
        return;
    }

    int idx = findTaskIndexById(tasks, *taskCount, id);
    if (idx == -1) {
        printf(COLOR_RED "Error: Task with ID #%d was not found." COLOR_RESET "\n", id);
        pausePrompt();
        return;
    }

    /* Show preview */
    renderTaskDetailCard(&tasks[idx], currentDate);

    /* Mandatory confirmation prompt */
    char confirm[32];
    printf("\n" COLOR_RED COLOR_BOLD "Are you sure you want to permanently delete Task #%d? (y/N): " COLOR_RESET, id);
    readString(confirm, sizeof(confirm));
    trimWhitespace(confirm);

    if (confirm[0] == 'y' || confirm[0] == 'Y') {
        if (deleteTask(tasks, taskCount, id)) {
            saveTasks(tasks, *taskCount, nextTaskId, DEFAULT_DATA_FILE);
            printf("\n" COLOR_GREEN "[SUCCESS] Task #%d permanently deleted from records." COLOR_RESET "\n", id);
        } else {
            printf("\n" COLOR_RED "[ERROR] Could not delete task." COLOR_RESET "\n");
        }
    } else {
        printf("\n" COLOR_GRAY "Deletion cancelled by user. Task #%d retained." COLOR_RESET "\n", id);
    }

    pausePrompt();
}

static void handleCompleteTask(Task tasks[], int taskCount, int nextTaskId, const char *currentDate) {
    clearScreen();
    renderBanner(currentDate);
    printf("\n" COLOR_BOLD COLOR_CYAN "=== FEATURE 6: COMPLETE TASK ===" COLOR_RESET "\n\n");

    if (taskCount == 0) {
        printf(COLOR_GRAY "No tasks available." COLOR_RESET "\n");
        pausePrompt();
        return;
    }

    printf("Enter Task ID to mark as COMPLETED:\n> ");
    int id = 0;
    if (!readInt(&id)) {
        printf(COLOR_RED "Invalid ID entered." COLOR_RESET "\n");
        pausePrompt();
        return;
    }

    int res = completeTask(tasks, taskCount, id);
    if (res == 1) {
        saveTasks(tasks, taskCount, nextTaskId, DEFAULT_DATA_FILE);
        printf("\n" COLOR_GREEN "[SUCCESS] Task #%d marked as COMPLETED!" COLOR_RESET "\n", id);
        int idx = findTaskIndexById(tasks, taskCount, id);
        if (idx != -1) {
            renderTaskDetailCard(&tasks[idx], currentDate);
        }
    } else if (res == 0) {
        printf("\n" COLOR_YELLOW "[NOTICE] Task #%d is ALREADY marked as COMPLETED! Duplicate operation prevented." COLOR_RESET "\n", id);
    } else {
        printf("\n" COLOR_RED "Error: Task with ID #%d was not found." COLOR_RESET "\n", id);
    }

    pausePrompt();
}

/* ========================================================================= */
/*                          APPLICATION ENTRY POINT                          */
/* ========================================================================= */

int main(void) {
    /* Initialize console: Virtual Terminal Processing for ANSI colors */
    initConsole();

    Task tasks[MAX_TASKS];
    int taskCount = 0;
    int nextTaskId = 1;

    /* Load stored tasks from binary file */
    int loadStatus = loadTasks(tasks, &taskCount, &nextTaskId, DEFAULT_DATA_FILE);

    char currentDate[DATE_LEN];
    getCurrentDateString(currentDate, sizeof(currentDate));

    /* Main Application Loop */
    int running = 1;
    while (running) {
        /* Refresh current date in case execution spans across midnight */
        getCurrentDateString(currentDate, sizeof(currentDate));

        clearScreen();
        renderBanner(currentDate);

        if (loadStatus == -1) {
            printf(COLOR_RED "\n[WARNING] %s was corrupted or unreadable. A fresh database has been initialized.\n" COLOR_RESET, DEFAULT_DATA_FILE);
            loadStatus = 0; /* Reset alert */
        }

        /* Render Live Dashboard with productivity metrics (Feature 16) */
        renderDashboard(tasks, taskCount, currentDate);

        /* Display Navigation Menu */
        renderMainMenu();

        int choice = -1;
        if (!readInt(&choice)) {
            printf("\n" COLOR_RED "Invalid selection! Please enter a number between 0 and 6." COLOR_RESET "\n");
            pausePrompt();
            continue;
        }

        switch (choice) {
            case 1: /* Add New Task */
                handleAddTask(tasks, &taskCount, &nextTaskId, currentDate);
                break;

            case 2: /* View All Tasks */
                handleViewAllTasks(tasks, taskCount, currentDate);
                break;

            case 3: /* Search Tasks */
                handleSearchTasks(tasks, taskCount, currentDate);
                break;

            case 4: /* Edit Task */
                handleEditTask(tasks, taskCount, nextTaskId, currentDate);
                break;

            case 5: /* Delete Task */
                handleDeleteTask(tasks, &taskCount, nextTaskId, currentDate);
                break;

            case 6: /* Mark Task as Completed */
                handleCompleteTask(tasks, taskCount, nextTaskId, currentDate);
                break;

            case 0: /* Save & Exit */
                saveTasks(tasks, taskCount, nextTaskId, DEFAULT_DATA_FILE);
                clearScreen();
                renderBanner(currentDate);
                printf("\n" COLOR_GREEN "All tasks safely persisted to %s." COLOR_RESET "\n", DEFAULT_DATA_FILE);
                printf(COLOR_CYAN COLOR_BOLD "\nThank you for using TASKZERO. Stay productive!\n\n" COLOR_RESET);
                running = 0;
                break;

            default:
                printf("\n" COLOR_RED "Invalid selection! Please choose an option between 0 and 6." COLOR_RESET "\n");
                pausePrompt();
                break;
        }
    }

    return 0;
}
