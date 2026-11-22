#include "task.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

/* ========================================================================= */
/*                          TASK MANAGEMENT OPERATIONS                       */
/* ========================================================================= */

int addTask(Task tasks[], int *taskCount, int *nextId,
            const char *title, const char *description,
            const char *dueDate, int priority) {
    if (tasks == NULL || taskCount == NULL || nextId == NULL) {
        return -1;
    }

    if (*taskCount >= MAX_TASKS) {
        return -1; /* Capacity reached */
    }

    int newId = (*nextId)++;
    int idx = *taskCount;

    tasks[idx].id = newId;

    /* Copy title safely */
    if (title != NULL && strlen(title) > 0) {
        strncpy(tasks[idx].title, title, TITLE_LEN - 1);
        tasks[idx].title[TITLE_LEN - 1] = '\0';
    } else {
        strncpy(tasks[idx].title, "Untitled Task", TITLE_LEN - 1);
        tasks[idx].title[TITLE_LEN - 1] = '\0';
    }

    /* Copy description safely */
    if (description != NULL && strlen(description) > 0) {
        strncpy(tasks[idx].description, description, DESC_LEN - 1);
        tasks[idx].description[DESC_LEN - 1] = '\0';
    } else {
        strncpy(tasks[idx].description, "No description provided.", DESC_LEN - 1);
        tasks[idx].description[DESC_LEN - 1] = '\0';
    }

    /* Copy due date */
    if (dueDate != NULL && strlen(dueDate) > 0) {
        strncpy(tasks[idx].dueDate, dueDate, DATE_LEN - 1);
        tasks[idx].dueDate[DATE_LEN - 1] = '\0';
    } else {
        getCurrentDateString(tasks[idx].dueDate, DATE_LEN);
    }

    /* Assign priority */
    if (priority >= PRIORITY_LOW && priority <= PRIORITY_HIGH) {
        tasks[idx].priority = priority;
    } else {
        tasks[idx].priority = PRIORITY_MEDIUM;
    }

    /* Initial status is always PENDING */
    tasks[idx].status = STATUS_PENDING;

    (*taskCount)++;
    return newId;
}

int findTaskIndexById(const Task tasks[], int taskCount, int id) {
    if (tasks == NULL || taskCount <= 0 || id <= 0) {
        return -1;
    }

    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].id == id) {
            return i;
        }
    }
    return -1;
}

int deleteTask(Task tasks[], int *taskCount, int id) {
    if (tasks == NULL || taskCount == NULL || *taskCount <= 0) {
        return 0;
    }

    int idx = findTaskIndexById(tasks, *taskCount, id);
    if (idx == -1) {
        return 0; /* Task not found */
    }

    /* Shift all elements left by one position to fill the gap */
    for (int i = idx; i < *taskCount - 1; i++) {
        tasks[i] = tasks[i + 1];
    }

    (*taskCount)--;
    return 1;
}

int completeTask(Task tasks[], int taskCount, int id) {
    if (tasks == NULL || taskCount <= 0) {
        return -1;
    }

    int idx = findTaskIndexById(tasks, taskCount, id);
    if (idx == -1) {
        return -1; /* Not found */
    }

    if (tasks[idx].status == STATUS_COMPLETED) {
        return 0; /* Already completed */
    }

    tasks[idx].status = STATUS_COMPLETED;
    return 1; /* Successfully completed */
}

int editTask(Task tasks[], int taskCount, int id,
             const char *newTitle, const char *newDesc, const char *newDueDate) {
    if (tasks == NULL || taskCount <= 0) {
        return -1;
    }

    int idx = findTaskIndexById(tasks, taskCount, id);
    if (idx == -1) {
        return -1; /* Not found */
    }

    if (newTitle != NULL && strlen(newTitle) > 0) {
        strncpy(tasks[idx].title, newTitle, TITLE_LEN - 1);
        tasks[idx].title[TITLE_LEN - 1] = '\0';
    }

    if (newDesc != NULL && strlen(newDesc) > 0) {
        strncpy(tasks[idx].description, newDesc, DESC_LEN - 1);
        tasks[idx].description[DESC_LEN - 1] = '\0';
    }

    if (newDueDate != NULL && strlen(newDueDate) > 0) {
        strncpy(tasks[idx].dueDate, newDueDate, DATE_LEN - 1);
        tasks[idx].dueDate[DATE_LEN - 1] = '\0';
    }

    return 1;
}

int searchTasks(const Task tasks[], int taskCount, const char *keyword, int matchIndices[]) {
    if (tasks == NULL || taskCount <= 0 || keyword == NULL || matchIndices == NULL) {
        return 0;
    }

    int matchCount = 0;
    for (int i = 0; i < taskCount; i++) {
        if (strCaseStr(tasks[i].title, keyword) || strCaseStr(tasks[i].description, keyword)) {
            matchIndices[matchCount++] = i;
        }
    }

    return matchCount;
}

int isTaskOverdue(const Task *task, const char *currentDate) {
    if (task == NULL || currentDate == NULL) {
        return 0;
    }

    /* Completed tasks are NEVER overdue */
    if (task->status == STATUS_COMPLETED) {
        return 0;
    }

    /* If pending and due date is earlier than current system date */
    return (compareDates(task->dueDate, currentDate) < 0);
}

double calculateProductivityScore(int completedTasks, int totalTasks) {
    if (totalTasks <= 0) {
        return 0.0;
    }
    return ((double)completedTasks / (double)totalTasks) * 100.0;
}

void getTaskStatistics(const Task tasks[], int taskCount, const char *currentDate,
                       int *total, int *completed, int *pending, int *overdue,
                       int *lowPriority, int *medPriority, int *highPriority) {
    *total = taskCount;
    *completed = 0;
    *pending = 0;
    *overdue = 0;
    *lowPriority = 0;
    *medPriority = 0;
    *highPriority = 0;

    for (int i = 0; i < taskCount; i++) {
        if (tasks[i].status == STATUS_COMPLETED) {
            (*completed)++;
        } else {
            (*pending)++;
            if (isTaskOverdue(&tasks[i], currentDate)) {
                (*overdue)++;
            }
        }

        if (tasks[i].priority == PRIORITY_LOW) {
            (*lowPriority)++;
        } else if (tasks[i].priority == PRIORITY_MEDIUM) {
            (*medPriority)++;
        } else if (tasks[i].priority == PRIORITY_HIGH) {
            (*highPriority)++;
        }
    }
}

const char* getPriorityName(int priority) {
    switch (priority) {
        case PRIORITY_LOW:    return "LOW";
        case PRIORITY_MEDIUM: return "MEDIUM";
        case PRIORITY_HIGH:   return "HIGH";
        default:              return "UNKNOWN";
    }
}

const char* getStatusName(int status) {
    switch (status) {
        case STATUS_COMPLETED: return "COMPLETED";
        case STATUS_PENDING:   return "PENDING";
        default:               return "UNKNOWN";
    }
}

const char* getPriorityColor(int priority) {
    switch (priority) {
        case PRIORITY_LOW:    return COLOR_BLUE;
        case PRIORITY_MEDIUM: return COLOR_YELLOW;
        case PRIORITY_HIGH:   return COLOR_RED;
        default:              return COLOR_WHITE;
    }
}

const char* getStatusColor(int status) {
    switch (status) {
        case STATUS_COMPLETED: return COLOR_GREEN;
        case STATUS_PENDING:   return COLOR_YELLOW;
        default:               return COLOR_WHITE;
    }
}

/* ========================================================================= */
/*                        BINARY FILE STORAGE OPERATIONS                     */
/* ========================================================================= */

int loadTasks(Task tasks[], int *taskCount, int *nextTaskId, const char *filename) {
    const char *filePath = (filename != NULL) ? filename : DEFAULT_DATA_FILE;

    *taskCount = 0;
    *nextTaskId = 1;

    FILE *fp = fopen(filePath, "rb");
    if (fp == NULL) {
        /* File does not exist yet; initialize empty database */
        return 0;
    }

    FileHeader header;
    size_t readHeader = fread(&header, sizeof(FileHeader), 1, fp);
    if (readHeader != 1) {
        fclose(fp);
        return -1; /* Corrupted header */
    }

    /* Check magic signature */
    if (memcmp(header.magic, FILE_MAGIC, 8) != 0) {
        fclose(fp);
        return -1; /* Invalid file format */
    }

    /* Validate reasonable values */
    if (header.taskCount < 0 || header.taskCount > MAX_TASKS || header.nextTaskId < 1) {
        fclose(fp);
        return -1; /* Corrupted metadata */
    }

    /* Read task records */
    if (header.taskCount > 0) {
        size_t readCount = fread(tasks, sizeof(Task), header.taskCount, fp);
        if (readCount != (size_t)header.taskCount) {
            fclose(fp);
            return -1; /* Incomplete task data */
        }
    }

    *taskCount = header.taskCount;
    *nextTaskId = header.nextTaskId;

    fclose(fp);
    return 1;
}

int saveTasks(const Task tasks[], int taskCount, int nextTaskId, const char *filename) {
    const char *filePath = (filename != NULL) ? filename : DEFAULT_DATA_FILE;

    if (taskCount < 0 || taskCount > MAX_TASKS) {
        return 0;
    }

    FILE *fp = fopen(filePath, "wb");
    if (fp == NULL) {
        return 0; /* Cannot open file for writing */
    }

    FileHeader header;
    memcpy(header.magic, FILE_MAGIC, 8);
    header.version = FILE_VERSION;
    header.taskCount = taskCount;
    header.nextTaskId = nextTaskId;

    if (fwrite(&header, sizeof(FileHeader), 1, fp) != 1) {
        fclose(fp);
        return 0;
    }

    if (taskCount > 0) {
        if (fwrite(tasks, sizeof(Task), taskCount, fp) != (size_t)taskCount) {
            fclose(fp);
            return 0;
        }
    }

    fflush(fp);
    fclose(fp);
    return 1;
}
