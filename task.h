#ifndef TASK_H
#define TASK_H

#define MAX_TASKS       100
#define TITLE_LEN       100
#define DESC_LEN        250
#define DATE_LEN        11

/* Priority Levels */
#define PRIORITY_LOW    1
#define PRIORITY_MEDIUM 2
#define PRIORITY_HIGH   3

/* Status Constants */
#define STATUS_PENDING   0
#define STATUS_COMPLETED 1

/* Storage Constants */
#define DEFAULT_DATA_FILE "tasks.dat"
#define FILE_MAGIC        "TASKZERO"
#define FILE_VERSION      1

/* Task Structure */
typedef struct {
    int id;
    char title[TITLE_LEN];
    char description[DESC_LEN];
    char dueDate[DATE_LEN];
    int priority;
    int status;
} Task;

/* Binary file header structure */
typedef struct {
    char magic[8];     /* "TASKZERO" */
    int version;       /* 1 */
    int taskCount;     /* Current active task count */
    int nextTaskId;    /* Auto-increment counter for unique IDs */
} FileHeader;

/* Task Management Function Prototypes */

/**
 * Adds a new task to the array.
 * Automatically assigns a unique ID from nextId and increments it.
 *
 * @return Assigned task ID on success, or -1 if capacity is reached.
 */
int addTask(Task tasks[], int *taskCount, int *nextId,
            const char *title, const char *description,
            const char *dueDate, int priority);

/**
 * Finds the array index of a task by its unique ID.
 *
 * @return Array index (0 to count-1), or -1 if not found.
 */
int findTaskIndexById(const Task tasks[], int taskCount, int id);

/**
 * Deletes a task by ID by shifting remaining tasks.
 * Does not reuse IDs.
 *
 * @return 1 on success, 0 if task ID not found.
 */
int deleteTask(Task tasks[], int *taskCount, int id);

/**
 * Marks a task as completed.
 *
 * @return 1 on success, 0 if already completed, -1 if ID not found.
 */
int completeTask(Task tasks[], int taskCount, int id);

/**
 * Edits the title, description, and due date of a task.
 *
 * @return 1 on success, -1 if ID not found.
 */
int editTask(Task tasks[], int taskCount, int id,
             const char *newTitle, const char *newDesc, const char *newDueDate);

/**
 * Searches tasks matching keyword in title or description (case-insensitive).
 *
 * @param matchIndices Output array to store matching indices.
 * @return Number of matching tasks found.
 */
int searchTasks(const Task tasks[], int taskCount, const char *keyword, int matchIndices[]);

/**
 * Checks whether a given task is overdue relative to the current system date.
 * Completed tasks are NEVER overdue.
 *
 * @return 1 if overdue, 0 otherwise.
 */
int isTaskOverdue(const Task *task, const char *currentDate);

/**
 * Calculates the productivity score as a percentage.
 * Formula: (Completed Tasks / Total Tasks) * 100
 *
 * @return Productivity score (0.0% to 100.0%). Returns 0.0 if total is 0.
 */
double calculateProductivityScore(int completedTasks, int totalTasks);

/**
 * Computes comprehensive task statistics.
 */
void getTaskStatistics(const Task tasks[], int taskCount, const char *currentDate,
                       int *total, int *completed, int *pending, int *overdue,
                       int *lowPriority, int *medPriority, int *highPriority);

/**
 * Returns string representation of a priority value.
 */
const char* getPriorityName(int priority);

/**
 * Returns string representation of a status value.
 */
const char* getStatusName(int status);

/**
 * Returns ANSI color code for a priority level.
 */
const char* getPriorityColor(int priority);

/**
 * Returns ANSI color code for a task status.
 */
const char* getStatusColor(int status);

/* Storage Function Prototypes */

/**
 * Loads tasks from the binary storage file into memory.
 * If file does not exist, initializes taskCount = 0 and nextTaskId = 1.
 *
 * @param tasks Output array of tasks
 * @param taskCount Output pointer to count of loaded tasks
 * @param nextTaskId Output pointer to next unique ID counter
 * @param filename File path to load from (pass NULL for DEFAULT_DATA_FILE)
 * @return 1 on successful load, 0 if new file initialized, -1 on corrupt file error.
 */
int loadTasks(Task tasks[], int *taskCount, int *nextTaskId, const char *filename);

/**
 * Saves tasks to the binary storage file.
 *
 * @param tasks Task array
 * @param taskCount Number of active tasks
 * @param nextTaskId Current next unique ID counter
 * @param filename File path to save to (pass NULL for DEFAULT_DATA_FILE)
 * @return 1 on success, 0 on file open/write error.
 */
int saveTasks(const Task tasks[], int taskCount, int nextTaskId, const char *filename);

#endif /* TASK_H */
