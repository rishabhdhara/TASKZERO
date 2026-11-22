# TASKZERO v1.0

**TASKZERO** is a console-based personal task management and
productivity tracking system written in C. It helps users organize
tasks, set due dates and priorities, track completion, and monitor
overall productivity through a live dashboard.

## Features

-   **Add tasks** with a title, optional description, due date, and
    priority.
-   **View all tasks** in a formatted table.
-   **Search tasks** by keyword or phrase in the task title or
    description. Search is case-insensitive.
-   **Edit tasks** by changing the title, description, or due date.
    Leave a field blank to keep its current value.
-   **Delete tasks** after a confirmation prompt.
-   **Mark tasks as completed** using their unique task ID.
-   **Automatic task IDs** that increment as tasks are created.
-   **Due-date validation** using the `YYYY-MM-DD` format and
    calendar-date checks.
-   **Overdue task detection** for pending tasks whose due dates have
    passed.
-   **Productivity dashboard** showing total, completed, pending, and
    overdue tasks, priority counts, and a completion percentage.
-   **Persistent storage** using a binary data file, so tasks remain
    available after the application closes.
-   **Colored terminal interface** using ANSI escape sequences.

## Menu

When TASKZERO starts, it displays a dashboard and the following menu:

  Option   Action
  -------- ------------------------
  `1`      Add New Task
  `2`      View All Tasks
  `3`      Search Tasks
  `4`      Edit Task
  `5`      Delete Task
  `6`      Mark Task as Completed
  `0`      Save & Exit

## Task Information

Each task stores:

  Field         Description
  ------------- ------------------------------------------------
  ID            Automatically generated unique task identifier
  Title         Task title (up to 99 characters)
  Description   Optional task details (up to 249 characters)
  Due Date      Deadline in `YYYY-MM-DD` format
  Priority      Low, Medium, or High
  Status        Pending or Completed

The application supports a maximum of **100 tasks**.

### Priority levels

-   `1` --- Low
-   `2` --- Medium
-   `3` --- High

If no priority is entered while creating a task, the default is
**Medium**.

If no due date is entered, the application uses the current system date.
If no description is entered, it stores `No description provided.`

## Productivity Score

The dashboard calculates the productivity score using:

``` text
Productivity Score = (Completed Tasks / Total Tasks) * 100
```

If there are no tasks, the score is `0%`.

A task is considered overdue when it is still pending and its due date
is earlier than the current system date. Completed tasks are not counted
as overdue.

## Project Structure

Keep the source and header files together in the same directory:

``` text
TASKZERO/
├── main.c       # Application entry point, dashboard, menu, and user interaction
├── task.c       # Task operations, statistics, and binary file storage
├── task.h       # Task structure, constants, and function declarations
├── utils.c      # Input, date, string, and terminal utility functions
├── utils.h      # Utility declarations and ANSI color definitions
├── tasks.dat    # Binary task database (created by the program)
├── TASKZERO.exe # Precompiled Windows executable, if supplied
└── README.md
```

`tasks.dat` is created in the program's working directory when task data
is saved. It is a binary file and is not intended to be edited manually.

## Requirements

-   A C compiler such as **GCC** or **MinGW GCC**.
-   A terminal or command prompt that supports standard input/output.
-   A terminal with ANSI escape-sequence support for the intended
    colored interface.

The project uses standard C library headers and does not require
third-party libraries.

## Build Instructions

### Windows (GCC / MinGW)

Open Command Prompt or PowerShell in the project directory and run:

``` bash
gcc main.c task.c utils.c -o TASKZERO.exe
```

Then run:

``` bash
TASKZERO.exe
```

### Linux / macOS (GCC or Clang)

Compile the source files together:

``` bash
gcc main.c task.c utils.c -o TASKZERO
```

Run:

``` bash
./TASKZERO
```

If your compiler uses a different command name, replace `gcc` with the
installed compiler command.

## Running the Supplied Executable

If the project includes `TASKZERO.exe`, it can be launched on a
compatible Windows system without compiling the source files.

For existing saved data to load, keep the database file named
**`tasks.dat`** in the program's working directory. Otherwise, TASKZERO
starts with an empty task list and creates the data file when it saves
tasks.

## Data Storage

TASKZERO stores records in `tasks.dat` using binary file handling
(`fread()` and `fwrite()`).

The file includes a header containing: - A file signature (`TASKZERO`) -
A file-format version - The number of stored tasks - The next task ID

The application checks the file header and stored task count when
loading. If the file is unreadable or its format is invalid, the program
displays a warning and initializes a fresh in-memory task list.

**Back up `tasks.dat`** if you want to preserve your task records before
replacing or moving project files.

## Input and Validation

-   Task titles cannot be empty.
-   Task titles and descriptions are trimmed of leading and trailing
    whitespace.
-   Due dates must be valid calendar dates in `YYYY-MM-DD` format.
-   The date validator accepts years from 2000 through 2100.
-   Priority must be `1`, `2`, or `3`.
-   Task IDs are used for editing, deleting, and completing tasks.
-   Deletion requires user confirmation.

## Source Files

-   `main.c` contains the application entry point, dashboard rendering,
    menu, and interactive feature handlers.
-   `task.c` implements task creation, searching, editing, deletion,
    completion, statistics, overdue checks, and binary file
    loading/saving.
-   `task.h` defines the `Task` and `FileHeader` structures, constants,
    and task-related function prototypes.
-   `utils.c` implements console helpers, safe line and integer input,
    whitespace trimming, date validation, date comparison, and
    case-insensitive substring search.
-   `utils.h` declares utility functions and defines terminal color
    codes.

## Notes

-   The task limit is fixed at 100, as defined by `MAX_TASKS`.
-   Task records are stored in a binary format; use the application to
    manage them rather than editing the file directly.
-   Keep `main.c`, `task.c`, `task.h`, `utils.c`, and `utils.h` together
    when compiling.
-   The program uses the system date to determine the current date and
    overdue tasks.