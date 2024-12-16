# University Management System

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
  - [Loading Professors](#loading-professors)
  - [Putting Grades](#putting-grades)
- [File Structure](#file-structure)
- [Data Files](#data-files)
  - [Professors File](#professors-file)
  - [Students File](#students-file)
- [Functions](#functions)
  - [loadProfessors()](#loadprofessors)
  - [putGrade()](#putgrade)
- [Error Handling](#error-handling)
- [Contributing](#contributing)
- [License](#license)

## Overview

The **University Management System** is a command-line based C program designed to manage professors, courses, and student grades within a university setting. It allows administrators to load professor data from a file, manage course assignments, and enable professors to assign grades to students enrolled in their courses.

## Features

- **Load Professors:** Read and parse professor details from a designated file.
- **Manage Courses:** Assign and display courses taught by each professor.
- **Assign Grades:** Allow professors to input grades for students in their respective courses.
- **Data Persistence:** Save updated student grades back to the data file.
- **User-Friendly Interface:** Interactive prompts guide users through various operations.

## Prerequisites

- **C Compiler:** Ensure you have a C compiler installed (e.g., `gcc`).
- **Standard C Libraries:** The program uses standard C libraries such as `stdio.h`, `stdlib.h`, `string.h`, and `ctype.h`.

## Installation

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/yourusername/university-management-system.git
   cd university-management-system
   ```

2. **Compile the Program:**

   Use `gcc` to compile the `code.c` file. Replace `code.c` with the actual filename if different.

   ```bash
   gcc -o university_mgmt code.c
   ```

   This command compiles `code.c` and outputs an executable named `university_mgmt`.

## Usage

After compiling, you can run the program using the terminal.

```bash
./university_mgmt
```

### Loading Professors

Upon running the program, it will attempt to load professor data from the specified professors file.

**Example:**

```
Loading professors from professors.txt...
Professors loaded successfully.
```

### Putting Grades

Professors can assign grades to students enrolled in their courses.

**Steps:**

1. **Select Professor:** Log in as a professor (authentication mechanism should be implemented as needed).
2. **View Courses:** The program displays a list of courses the professor is teaching.
3. **Select Course:** Choose a course to assign grades.
4. **View Students:** The program lists all students enrolled in the selected course.
5. **Select Student:** Choose a student to assign a grade.
6. **Enter Grade:** Input the grade (e.g., A+, A0, B+, etc.).
7. **Save Grade:** The grade is saved and persisted to the student data file.

**Example Interaction:**

```
--- Put Grade ---
Courses you are teaching:
1. Introduction to Computer Science
2. Data Structures

Enter the number of the course to put grades for: 1

Students taking Introduction to Computer Science:
1. Alice Johnson (S001)
2. Bob Smith (S002)

Enter the number of the student to put grade for: 1

Enter the grade (A+, A0, B+, B0, C+, C0, D+, D0): A+
Grade updated successfully.
```

## File Structure

```
university-management-system/
├── code.c
├── professors.txt
├── students.txt
├── README.md
└── Makefile
```

- **code.c:** Main C source file containing the program logic.
- **professors.txt:** Data file containing professor details.
- **students.txt:** Data file containing student details and their enrolled courses.
- **README.md:** Documentation file (this file).
- **Makefile:** Optional file to automate the build process.

## Data Files

### Professors File (`professors.txt`)

Contains information about professors, including their ID, name, major, email, phone, password, and courses they teach.

**Format:**

```
ID: P123, Name: John Doe, Major: ComputerScience, Email: johndoe@example.com, Phone: 123-456-7890, Password: secret, Courses: CS101, CS201, CS301
ID: P124, Name: Jane Smith, Major: Mathematics, Email: janesmith@example.com, Phone: 987-654-3210, Password: password, Courses: MA101, MA201
```

### Students File (`students.txt`)

Contains information about students, including their ID, name, enrolled courses, and grades.

**Format:**

```
ID: S001, Name: Alice Johnson, Courses: CS101, MA101, Grades: 4.5, 3.0
ID: S002, Name: Bob Smith, Courses: CS201, CS301, Grades: 3.5, 4.0
```

*Ensure that the order of courses and grades corresponds correctly.*

## Functions

### `loadProfessors()`

**Purpose:**  
Loads professor data from the `professors.txt` file into the program's data structures.

**Functionality:**

1. **Open File:**  
   Opens the professors file in read mode. If the file cannot be opened, an error message is displayed, and the function exits.

2. **Read and Parse Lines:**  
   - Reads each line from the file.
   - Removes the newline character at the end of each line.
   - Splits the line into tokens based on commas.
   - Extracts and stores each field (ID, Name, Major, Email, Phone, Password).
   - Extracts the list of courses taught by the professor.

3. **Store Data:**  
   Populates the `professors` array with the parsed data and increments the `professorCount`.

**Corrected Implementation:**

```c
void loadProfessors() {
    FILE *fp = fopen(PROFESSORS_FILE, "r");
    if (!fp) {
        perror("File opening failed");
        printf("Error opening %s\n", PROFESSORS_FILE);
        return;
    }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0'; // Remove newline character

        // Split the line by commas
        char *tokens[50];  // Assuming a safe upper limit for number of tokens
        int tokenCount = 0;
        char *temp = strtok(line, ",");
        while (temp && tokenCount < 50) {
            // Trim leading and trailing spaces from the token
            while (isspace((unsigned char)*temp)) temp++;
            char *end = temp + strlen(temp) - 1;
            while (end > temp && isspace((unsigned char)*end)) end--;
            *(end + 1) = '\0';

            tokens[tokenCount++] = temp;
            temp = strtok(NULL, ",");
        }

        if (tokenCount < 6) {
            // If we don't have at least 6 tokens, the data line is incomplete
            continue;
        }

        // Parse the mandatory fields
        sscanf(tokens[0], "ID: %s", professors[professorCount].id);
        sscanf(tokens[1], "Name: %[^\n]", professors[professorCount].name);
        sscanf(tokens[2], "Major: %s", professors[professorCount].major);
        sscanf(tokens[3], "Email: %s", professors[professorCount].email);
        sscanf(tokens[4], "Phone: %s", professors[professorCount].phone);
        sscanf(tokens[5], "Password: %s", professors[professorCount].password);

        // Parse courses (if any)
        professors[professorCount].courseCount = 0;
        if (tokenCount > 6) {
            // The first courses-related token should contain "Courses:"
            char *courseToken = tokens[6];
            char *coursesPtr = strstr(courseToken, "Courses:");
            if (coursesPtr) {
                // Move pointer past "Courses:"
                coursesPtr += strlen("Courses:");
                // Trim spaces
                while (isspace((unsigned char)*coursesPtr)) coursesPtr++;

                // Store first course after "Courses:"
                if (*coursesPtr != '\0' && professors[professorCount].courseCount < 6) {
                    strncpy(professors[professorCount].courses[professors[professorCount].courseCount], coursesPtr, MAX_STRING);
                    professors[professorCount].courses[professors[professorCount].courseCount][MAX_STRING-1] = '\0';
                    professors[professorCount].courseCount++;
                }

                // Subsequent tokens (tokens[7], tokens[8], ...) are additional courses
                for (int i = 7; i < tokenCount && professors[professorCount].courseCount < 6; i++) {
                    strncpy(professors[professorCount].courses[professors[professorCount].courseCount], tokens[i], MAX_STRING);
                    professors[professorCount].courses[professors[professorCount].courseCount][MAX_STRING-1] = '\0';
                    professors[professorCount].courseCount++;
                }
            }
        }

        professorCount++;
    }

    fclose(fp);
}
```

### `putGrade()`

**Purpose:**  
Allows a professor to assign grades to students enrolled in their courses.

**Functionality:**

1. **Display Courses:**  
   Lists all courses the professor is teaching.

2. **Select Course:**  
   Prompts the professor to select a course to assign grades.

3. **List Students:**  
   Displays all students enrolled in the selected course.

4. **Select Student:**  
   Prompts the professor to select a student to assign a grade.

5. **Enter Grade:**  
   Allows the professor to input a grade for the selected student.

6. **Save Grade:**  
   Updates the student's grade and saves the changes to the data file.

**Implementation:**

```c
void putGrade(Professor *prof) {
    printf("\n--- Put Grade ---\n");
    printf("Courses you are teaching:\n");
    for (int i = 0; i < prof->courseCount; i++) {
        char* courseName = getCourseNameByCode(prof->courses[i]);
        if (courseName) {
            printf("%d. %s\n", i+1, courseName);
        } else {
            printf("%d. %s (Course name not found)\n", i+1, prof->courses[i]);
        }
    }
    if (prof->courseCount == 0) {
        printf("You are not teaching any courses.\n");
        return;
    }
    printf("Enter the number of the course to put grades for: ");
    int courseChoice;
    scanf("%d", &courseChoice);
    getchar(); // consume newline

    if (courseChoice < 1 || courseChoice > prof->courseCount) {
        printf("Invalid choice.\n");
        return;
    }
    char *selectedCourseCode = prof->courses[courseChoice - 1];
    char *selectedCourseName = getCourseNameByCode(selectedCourseCode);

    if (!selectedCourseName) {
        printf("Course name not found for course code %s.\n", selectedCourseCode);
        return;
    }

    Student *studentsInCourse[MAX_STUDENTS];
    int studentsInCourseCount = 0;
    for (int i = 0; i < studentCount; i++) {
        for (int j = 0; j < 6; j++) {
            if (strcmp(students[i].courses[j], selectedCourseName) == 0) {
                studentsInCourse[studentsInCourseCount++] = &students[i];
                break;
            }
        }
    }
    if (studentsInCourseCount == 0) {
        printf("No students are taking this course.\n");
        return;
    }

    printf("Students taking %s:\n", selectedCourseName);
    for (int i = 0; i < studentsInCourseCount; i++) {
        printf("%d. %s (%s)\n", i+1, studentsInCourse[i]->name, studentsInCourse[i]->id);
    }
    printf("Enter the number of the student to put grade for: ");
    int studentChoice;
    scanf("%d", &studentChoice);
    getchar(); // consume newline

    if (studentChoice < 1 || studentChoice > studentsInCourseCount) {
        printf("Invalid choice.\n");
        return;
    }

    Student *selectedStudent = studentsInCourse[studentChoice - 1];

    printf("Enter the grade (A+, A0, B+, B0, C+, C0, D+, D0): ");
    char gradeInput[MAX_STRING];
    fgets(gradeInput, MAX_STRING, stdin);
    gradeInput[strcspn(gradeInput, "\n")] = '\0'; // remove newline

    double gradeValue;
    if (strcmp(gradeInput, "A+") == 0) gradeValue = 4.5;
    else if (strcmp(gradeInput, "A0") == 0) gradeValue = 4.0;
    else if (strcmp(gradeInput, "B+") == 0) gradeValue = 3.5;
    else if (strcmp(gradeInput, "B0") == 0) gradeValue = 3.0;
    else if (strcmp(gradeInput, "C+") == 0) gradeValue = 2.5;
    else if (strcmp(gradeInput, "C0") == 0) gradeValue = 2.0;
    else if (strcmp(gradeInput, "D+") == 0) gradeValue = 1.5;
    else if (strcmp(gradeInput, "D0") == 0) gradeValue = 1.0;
    else {
        printf("Invalid grade input.\n");
        return;
    }

    int courseIndex = -1;
    for (int i = 0; i < 6; i++) {
        if (strcmp(selectedStudent->courses[i], selectedCourseName) == 0) {
            courseIndex = i;
            break;
        }
    }
    if (courseIndex == -1) {
        printf("Error: Student does not have this course.\n");
        return;
    }

    selectedStudent->grades[courseIndex] = gradeValue;
    saveStudentCourses();
    printf("Grade updated successfully.\n");
}
```

## Error Handling

The program includes error handling to manage common issues:

- **File Opening Errors:**  
  If the program fails to open the required data files (`professors.txt`, `students.txt`), it will display an error message and exit gracefully.

- **Invalid Input:**  
  When prompting for course or student selections, the program checks for invalid choices and notifies the user accordingly.

- **Data Parsing Errors:**  
  If the data lines in the input files are incomplete or malformed, the program skips those entries and continues processing the rest.

## Contributing

Contributions are welcome! If you'd like to improve the University Management System, please follow these steps:

1. **Fork the Repository:**  
   Click the "Fork" button at the top right of this page to create a copy of the repository.

2. **Create a Branch:**  
   ```bash
   git checkout -b feature/YourFeature
   ```

3. **Make Changes:**  
   Implement your feature or bug fix.

4. **Commit Changes:**  
   ```bash
   git commit -m "Add feature: YourFeature"
   ```

5. **Push to Branch:**  
   ```bash
   git push origin feature/YourFeature
   ```

6. **Create a Pull Request:**  
   Navigate to the original repository and click "Compare & pull request."

Please ensure your code follows the project's coding standards and includes appropriate comments.

## License

This project is licensed under the [MIT License](LICENSE).
