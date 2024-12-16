#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_LINE 1024
#define MAX_COURSES 500
#define MAX_STUDENTS 1000
#define MAX_PROFESSORS 50
#define MAX_STRING 100

#if defined(_WIN32) || defined(_WIN64)
    #include <conio.h>
#else
    #include <termios.h>
    #include <unistd.h>
#endif

char STUDENTS_FILE[] = "students.txt";
char PROFESSORS_FILE[] = "professors.txt";
char STUDENT_COURSES_FILE[] = "student_classes.txt";
char COURSES_FILE[] = "courses.txt";

char adminLogin[] = "admin";
char adminPwd[] = "admin123";

typedef struct {
    char courseName[MAX_STRING];
    char courseCode[MAX_STRING];
} Course;

typedef struct {
    char id[MAX_STRING];
    char name[MAX_STRING];
    char faculty[MAX_STRING];
    char advisorName[MAX_STRING];
    char email[MAX_STRING];
    char phoneNumber[MAX_STRING];
    char dateOfBirth[MAX_STRING];
    char gender;
    char address[MAX_STRING];
    int scholarshipAmount; // Percentage (e.g., 100, 50)
    char clubsAttending[MAX_STRING]; // "None" or list of clubs
    char password[MAX_STRING];
    char nationality[MAX_STRING];
    char advisorID[MAX_STRING];
    int semesters;

    char courses[6][MAX_STRING];  // Array to store up to 6 course names
    double grades[6];  
} Student;

typedef struct {
    char id[MAX_STRING], name[MAX_STRING], major[MAX_STRING], email[MAX_STRING], phone[MAX_STRING], password[MAX_STRING], courses[6][MAX_STRING];
    int courseCount;
} Professor;

// Global arrays to hold data
Course courses[MAX_COURSES];
Student students[MAX_STUDENTS];
Professor professors[MAX_PROFESSORS];

int courseCount = 0, studentCount = 0, professorCount = 0;

void loadStudents();
void loadProfessors();
void loadStudentCourses();
void loadCourses();
void saveStudents();
void saveProfessors();
void saveStudentCourses();
void mainMenu();
void adminMenu();
void professorMenu(Professor *prof);
void studentMenu(Student *stud);
int login(char *role, char *id, char *password);
void addNewStudent();
void deleteStudent();
void viewInformationAdmin();
void changeInformationAdmin();
void encryptData();
void decryptData();
void changeProfessorInfo(Professor *prof);
void viewStudentsProfessor(Professor *prof);
void generateStudentReport();
void filterStudentsProfessor();
void viewStudentInfo(Student *stud);
void changeStudentInfo(Student *stud);
void putGrade(Professor *prof);
int validateEmail(char *email);
int validatePhoneNumber(char *phone);
int validatePassword(char *password);
int validateName(char *name);
int validateBirthday(char *birthday);
int validateMajor(char *major);
void encrypt(char *text, int key);
void decrypt(char *text, int key);
char* getCourseNameByCode(char* code);
char* getCourseCodeByName(char* name);
void getPassword(char *password, size_t max_len);
void displayWelcomeMessage();

int main() {
    displayWelcomeMessage();
    loadCourses();
    loadStudents();
    loadProfessors();
    loadStudentCourses(); // Load student courses and grades
    mainMenu();
    return 0;
}

void mainMenu() {
    int choice;
    char id[MAX_STRING], password[MAX_STRING];
    while (1) {
        printf("\n=================================\n");
        printf("            \033[1;33mMAIN MENU\033[0m\n");
        printf("=================================\n");
        printf("1. Login as Admin\n");
        printf("2. Login as Professor\n");
        printf("3. Login as Student\n");
        printf("4. Exit\n");
        printf("=================================\n");
        printf("\033[1;33mEnter your choice:\033[0m ");
        if (scanf("%d", &choice) != 1) {
            printf("\033[1;31mInvalid input. Please enter a number.\033[0m\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        getchar(); // Consume newline
        if (choice == 4) {
            printf("\033[0;31mSaving Changes and Exiting\033[0m");
            for (int i = 0; i < 3; i++) {
                printf("\033[0;31m.\033[0m");
                fflush(stdout);
                sleep(1);}
            printf("\n");

            break;
        }
        printf("\033[1;33mEnter ID:\033[0m ");
        fgets(id, MAX_STRING, stdin);
        id[strcspn(id, "\n")] = '\0'; // Remove newline

        printf("\033[1;33mEnter Password:\033[0m ");
        getPassword(password, MAX_STRING);

        if (choice == 1) {
            if (strcmp(id, adminLogin) == 0 && strcmp(password, adminPwd) == 0)
                adminMenu();
            else 
                printf("\033[1;31mInvalid admin credentials.\033[0m\n");
        } else if (choice == 2) {
            int idx = login("professor", id, password);
            if (idx != -1)
                professorMenu(&professors[idx]);
            else
                printf("\033[1;31mInvalid professor credentials.\033[0m\n");
        } else if (choice == 3) {
            int idx = login("student", id, password);
            if (idx != -1)
                studentMenu(&students[idx]);
            else
                printf("\033[1;31mInvalid student credentials.\033[0m\n");
        }
        else
            printf("\033[1;31mInvalid choice. Try again.\033[0m\n");
    }
}


void loadStudentCourses() {
    FILE *fp = fopen(STUDENT_COURSES_FILE, "r");
    if (!fp) {
        perror("\033[1;31mFile opening failed\033[0m");
        printf("\033[1;31mError opening\033[0m %s\n", STUDENT_COURSES_FILE);
        return;
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0';           // Remove newline character at the end if present
        char *studentID = strtok(line, ":");            // Split the line into student ID, courses, and grades
        char *coursesStr = strtok(NULL, ":");
        char *gradesStr = strtok(NULL, ":");

        if (!studentID || !coursesStr || !gradesStr) {
            printf("\033[1;31mInvalid format\033[0m in %s\n", STUDENT_COURSES_FILE);
            continue;
        }
        int found = 0;  // Find the student by ID
        for (int i = 0; i < studentCount; i++) {
            if (strcmp(students[i].id, studentID) == 0) {
                found = 1;
                char *course = strtok(coursesStr, ",");                 // split or read courses
                int courseIdx = 0;
                while (course && courseIdx < 6) {
                    strcpy(students[i].courses[courseIdx], course);
                    course = strtok(NULL, ",");
                    courseIdx++;
                }
                char *grade = strtok(gradesStr, ",");           //split or read grades
                int gradeIdx = 0;
                while (grade && gradeIdx < 6) {
                    students[i].grades[gradeIdx] = atof(grade);
                    grade = strtok(NULL, ",");
                    gradeIdx++;
                }
                break;
            }
        }
        if (!found)
        printf("Student ID %s not found in students list.\n", studentID);
    }
    fclose(fp);
}

void loadCourses() {
    FILE *fp = fopen(COURSES_FILE, "r");
    if (!fp) {
        perror("\033[1;31mFile opening failed\033[0m");
        printf("\033[1;31mError opening\033[0m %s\n", COURSES_FILE);
        return;
    }
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\n")] = '\0'; // Remove newline character
        char *name = strtok(line, ",");
        char *code = strtok(NULL, ",");
        if (name && code) {
            strcpy(courses[courseCount].courseName, name);
            strcpy(courses[courseCount].courseCode, code);
            courseCount++;
            if (courseCount >= MAX_COURSES) {
                printf("Reached maximum courses limit.\n");
                break;
            }
        }
    }
    fclose(fp);
}

void loadStudents() {
    FILE *fp = fopen(STUDENTS_FILE, "r");
    if (!fp) {
        perror("\033[1;31mFile opening failed\033[0m");
        printf("\033[1;31mError opening\033[0m %s\n", STUDENTS_FILE);
        return;
    }
    char line[MAX_LINE];
    fgets(line, sizeof(line), fp); // Skip header
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%c\t%s\t%d\t%d\t%s\t%s\t%s\t%s",
            students[studentCount].id,
            students[studentCount].name,
            students[studentCount].faculty,
            students[studentCount].advisorName,
            students[studentCount].email,
            students[studentCount].phoneNumber,
            students[studentCount].dateOfBirth,
            &students[studentCount].gender,
            students[studentCount].address,
            &students[studentCount].semesters,
            &students[studentCount].scholarshipAmount,
            students[studentCount].clubsAttending,
            students[studentCount].password,
            students[studentCount].nationality,
            students[studentCount].advisorID);
        studentCount++;
    }
    fclose(fp);
}

void loadProfessors() {
    FILE *fp = fopen(PROFESSORS_FILE, "r");
    if (!fp) {
        perror("\033[1;31mFile opening failed\033[0m");
        printf("\033[1;31mError opening\033[0m %s\n", PROFESSORS_FILE);
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



void saveStudents() {
    FILE *fp = fopen(STUDENTS_FILE, "w");
    if (!fp) {
        perror("\033[1;31mFile opening failed\033[0m");
        printf("\033[1;31mError opening\033[0m %s for writing.\n", STUDENTS_FILE);
        return;
    }
    fprintf(fp, "Student ID\tName\tFaculty\tAdvisor's Name\tEmail\tPhone Number\tDate of Birth\tGender\tAddress\tSemesters\tScholarship Amount\tClubs Attending\tPassword\tNationality\tAdvisor's ID\n");
    for (int i = 0; i < studentCount; i++) {
        fprintf(fp, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%c\t%s\t%d\t%d\t%s\t%s\t%s\t%s\n",
                students[i].id,
                students[i].name,
                students[i].faculty,
                students[i].advisorName,
                students[i].email,
                students[i].phoneNumber,
                students[i].dateOfBirth,
                students[i].gender, 
                students[i].address,
                students[i].semesters,
                students[i].scholarshipAmount,
                students[i].clubsAttending,
                students[i].password,
                students[i].nationality,
                students[i].advisorID);
    }
    fclose(fp);
}

void saveStudentCourses() {
    FILE *fp = fopen(STUDENT_COURSES_FILE, "w");
    if (!fp) {
        perror("\033[1;31mFile opening failed\033[0m");
        printf("\033[1;31mError opening\033[0m %s for writing.\n", STUDENT_COURSES_FILE);
        return;
    }
    for (int i = 0; i < studentCount; i++) {
        if (strlen(students[i].courses[0]) == 0) {          //skipping students if they are not taking any courses
            continue;
        }
        fprintf(fp, "%s:", students[i].id);
        for (int j = 0; j < 6; j++) {                       //writing courses
            if (strlen(students[i].courses[j]) == 0)
                break;
            fprintf(fp, "%s", students[i].courses[j]);
            if (j < 5 && strlen(students[i].courses[j+1]) != 0)
                fprintf(fp, ",");
        }
        fprintf(fp, ":");
        for (int j = 0; j < 6; j++) {
            if (strlen(students[i].courses[j]) == 0)
                break;
            fprintf(fp, "%.1f", students[i].grades[j]);
            if (j < 5 && strlen(students[i].courses[j+1]) != 0)
                fprintf(fp, ",");
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}


void saveProfessors() {
    FILE *fp = fopen(PROFESSORS_FILE, "w");
    if (!fp) {
        perror("\033[1;31mFile opening failed\033[0m");
        printf("\033[1;31mError opening\033[0m %s for writing.\n", PROFESSORS_FILE);
        return;
    }
    for (int i = 0; i < professorCount; i++) {
        fprintf(fp, "ID: %s, Name: %s, Major: %s, Email: %s, Phone: %s, Password: %s, Courses:", 
                professors[i].id,
                professors[i].name,
                professors[i].major,
                professors[i].email,
                professors[i].phone,
                professors[i].password);
        for (int j = 0; j < professors[i].courseCount; j++) {
            fprintf(fp, " %s", professors[i].courses[j]);
            if (j < professors[i].courseCount - 1) {
                fprintf(fp, ",");
            }
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

int login(char *role, char *id, char *password) {
    if (strcmp(role, "student") == 0) {
        for (int i = 0; i < studentCount; i++) {
            if (strcmp(students[i].id, id) == 0 && strcmp(students[i].password, password) == 0) {
                return i;
            }
        }
    } else if (strcmp(role, "professor") == 0) {
        for (int i = 0; i < professorCount; i++) {
            if (strcmp(professors[i].id, id) == 0 && strcmp(professors[i].password, password) == 0) {
                return i;
            }
        }
    }
    return -1;
}

const char* getLetterGrade(double grade) {
    if (grade >= 4.5) return "A+";
    else if (grade >= 4.0) return "A0";
    else if (grade >= 3.5) return "B+";
    else if (grade >= 3.0) return "B0";
    else if (grade >= 2.5) return "C+";
    else if (grade >= 2.0) return "C0";
    else if (grade >= 1.5) return "D+";
    else if (grade >= 1.0) return "D0";
    else return "F";
}

void displayCoursesAndGrades(Student *stud) {
    printf("\n--- \033[1;33mCourses and Grades\033[0m ---\n");
    printf("Course\t\tGrade\n");
    printf("---------------------------\n");
    for (int i = 0; i < 6; i++) {
        if (strlen(stud->courses[i]) == 0)      //if no more courses
            break;
        printf("%-25s %s\n", stud->courses[i], getLetterGrade(stud->grades[i]));
    }
}

double calculateGPA(Student *stud) {
    double total = 0.0;
    int count = 0;
    for (int i = 0; i < 6; i++) {
        if (stud->grades[i] > 0.0) { // Assuming a grade of 0.0 means no grade assigned
            total += stud->grades[i];
            count++;
        }
    }
    if (count == 0) return 0.0; // Avoid division by zero
    return total / count;
}

void adminMenu() {
    int choice;
    while (1) {
        printf("\n--- \033[1;33mAdmin Menu\033[0m ---\n1. View Information\n2. Change Information\n3. Encrypt Data\n4. Decrypt Data\n5. Add New Student\n6. Delete Student\n7. Logout\n\033[1;33mEnter your choice:\033[0m ");
        scanf("%d", &choice);
        getchar(); // Consume newline
        if (choice == 7) {
            printf("\033[1;31mLogging out\033[0m");
            for (int i = 0; i <3; i++) {
                printf("\033[1;31m.\033[0m");
                fflush(stdout);
                sleep(1);}
            printf("\n");
            break;
        }
        switch (choice) {
            case 1:
                viewInformationAdmin();
                break;
            case 2:
                changeInformationAdmin();
                break;
            case 3:
                encryptData();
                break;
            case 4:
                decryptData();
                break;
            case 5:
                addNewStudent();
                break;
            case 6:
                deleteStudent();
                break;
            default:
                printf("\033[1;30mInvalid choice. Try again.\033[0m\n");
        }
    }
}

void professorMenu(Professor *prof) {
    int choice;
    while (1) {
        printf("\n--- Professor Menu ---\n1. Change Email/Phone Number\n2. View All Students' Information\n3. Generate Student Report\n4. Filter Students\n5. Add New Student\n6. Put Grade\n7. Logout\n\033[1;33mEnter your choice:\033[0m ");
        scanf("%d", &choice);
        getchar(); // Consume newline
        if (choice == 7) {
            printf("Logging out");
            for (int i = 0; i <3; i++) {
                printf(".");
                fflush(stdout);
                sleep(1);}
            printf("\n");
            break;
        }
        switch (choice) {
            case 1:
                changeProfessorInfo(prof);
                break;
            case 2:
                viewStudentsProfessor(prof);
                break;
            case 3:
                generateStudentReport();
                break;
            case 4:
                filterStudentsProfessor();
                break;
            case 5:
                addNewStudent();
                break;
            case 6:
                putGrade(prof);
                break;
            default:
                printf("\033[1;30mInvalid choice. Try again.\033[0m\n");
        }
    }
}

void studentMenu(Student *stud) {
    int choice;
    while (1) {
        printf("\n--- Student Menu ---\n1. View My Information\n2. Change Email/Phone Number\n3. Logout\n\033[1;33mEnter your choice:\033[0m ");
        scanf("%d", &choice);
        getchar(); // Consume newline
        if (choice == 3) {
            printf("Logging out");
            for (int i = 0; i <3; i++) {
                printf(".");
                fflush(stdout);
                sleep(1);}
            printf("\n");
            break;
        }
        switch (choice) {
            case 1:
                viewStudentInfo(stud);
                break;
            case 2:
                changeStudentInfo(stud);
                break;
            default:
                printf("\033[1;30mInvalid choice. Try again.\033[0m\n");
        }
    }
}

void addNewStudent() {
    if (studentCount >= MAX_STUDENTS) {
        printf("Maximum number of students reached. Cannot add more.\n");
        return;
    }
    Student newStudent;
    char temp[MAX_STRING]; // Temporary buffer for input

    printf("\033[1;33mEnter Name:\033[0m ");         //all inputs are listed below
    fgets(newStudent.name, MAX_STRING, stdin);
    newStudent.name[strcspn(newStudent.name, "\n")] = '\0';
    if (!validateName(newStudent.name)) {
        printf("\033[1;31mInvalid name. Only alphabetic characters are allowed.\033[0m\n");
        return;
    }
    printf("\033[1;33mEnter Email:\033[0m ");
    fgets(newStudent.email, MAX_STRING, stdin);
    newStudent.email[strcspn(newStudent.email, "\n")] = '\0';
    if (!validateEmail(newStudent.email)) {
        printf("\033[1;31mInvalid email. Must contain '@inha.edu'.\n");
        return;
    }
    printf("\033[1;33mEnter Phone Number:\033[0m ");
    fgets(newStudent.phoneNumber, MAX_STRING, stdin);
    newStudent.phoneNumber[strcspn(newStudent.phoneNumber, "\n")] = '\0';
    if (!validatePhoneNumber(newStudent.phoneNumber)) {
        printf("\033[1;31mInvalid phone number. Must be 11 digits starting with '010'.\n");
        return;
    }
    printf("\033[1;33mEnter Major (IBT, ISE, KLC):\033[0m ");
    fgets(newStudent.faculty, MAX_STRING, stdin);
    newStudent.faculty[strcspn(newStudent.faculty, "\n")] = '\0';
    if (!validateMajor(newStudent.faculty)) {
        printf("\033[1;31mInvalid major. Choose from IBT, ISE, KLC.\n");
        return;
    }
    printf("\033[1;33mEnter Password (10 characters including numbers and special sybmols):\033[0m ");
    getPassword(newStudent.password, MAX_STRING);
    newStudent.password[strcspn(newStudent.password, "\n")] = '\0';
    if (!validatePassword(newStudent.password)) {
        printf("\033[1;31mInvalid password. Must be exactly 10 characters.\n");
        return;
    }
    printf("\033[1;33mEnter Date of Birth (yyyymmdd):\033[0m ");
    fgets(newStudent.dateOfBirth, MAX_STRING, stdin);
    newStudent.dateOfBirth[strcspn(newStudent.dateOfBirth, "\n")] = '\0';
    if (!validateBirthday(newStudent.dateOfBirth)) {
        printf("\033[1;31mInvalid date of birth. Must be in 'yyyymmdd' format.\033[0m\n");
        return;
    }
    printf("\033[1;33mEnter Address:\033[0m ");
    fgets(newStudent.address, MAX_STRING, stdin);
    newStudent.address[strcspn(newStudent.address, "\n")] = '\0';
    while (1) {
        printf("\033[1;33mEnter Gender (M/F/U):\033[0m ");
        fgets(temp, MAX_STRING, stdin);
        temp[strcspn(temp, "\n")] = '\0';
        if (strlen(temp) == 1 && (toupper(temp[0]) == 'M' || toupper(temp[0]) == 'F' || toupper(temp[0]) == 'U')) {
            newStudent.gender = toupper(temp[0]);
            break;
        } else {
            printf("\033[1;31mInvalid input. Please enter 'M' for Male, 'F' for Female, or 'U' for Unspecified.\033[0m\n");
        }
    }
    while (1) {
        printf("\033[1;33mEnter Scholarship Amount (percentage, e.g., 100 or 50):\033[0m ");
        fgets(temp, MAX_STRING, stdin);
        newStudent.scholarshipAmount = atoi(temp);
        if (newStudent.scholarshipAmount >= 0 && newStudent.scholarshipAmount <= 100) {
            break;
        } else {
            printf("\033[1;31mInvalid input. Please enter a percentage between 0 and 100.\033[0m\n");
        }
    }
    printf("\033[1;33mEnter Nationality:\033[0m ");
    fgets(newStudent.nationality, MAX_STRING, stdin);
    newStudent.nationality[strcspn(newStudent.nationality, "\n")] = '\0';
    if (strlen(newStudent.nationality) == 0) {
        strcpy(newStudent.nationality, "Unknown");
    }
    while (1) {
        printf("Is the student attending any clubs? (Y/N):\033[0m ");
        fgets(temp, MAX_STRING, stdin);
        temp[strcspn(temp, "\n")] = '\0';
        if (strcasecmp(temp, "Y") == 0) {
            printf("\033[1;33mEnter Club Name:\033[0m ");
            fgets(newStudent.clubsAttending, MAX_STRING, stdin);
            newStudent.clubsAttending[strcspn(newStudent.clubsAttending, "\n")] = '\0';
            if (strlen(newStudent.clubsAttending) == 0) {
                strcpy(newStudent.clubsAttending, "None");
            }
            break;
        } else if (strcasecmp(temp, "N") == 0) {
            strcpy(newStudent.clubsAttending, "None");
            break;
        } else {
            printf("\033[1;31mInvalid input. Please enter 'Y' for Yes or 'N' for No.\033[0m\n");
        }
    }

    int lastID = 0;                                   //there is no option to choose unique id, it will be automatically generated by adding 1 to the last in the list
    for (int i = 0; i < studentCount; i++) {
        int idNum = atoi(students[i].id);
        if (idNum > lastID) {
            lastID = idNum;
        }
    }
    lastID++;
    sprintf(newStudent.id, "%d", lastID);
    newStudent.semesters = 1;

    int profIndices[MAX_PROFESSORS];                 //advisor professor is assigned randomly
    int profCount = 0;
    for (int i = 0; i < professorCount; i++) {
        if (strcmp(professors[i].major, newStudent.faculty) == 0) {
            profIndices[profCount++] = i;
        }
    }
    if (profCount == 0) {
        printf("No professors available for this major. Cannot assign advisor.\n");
        return;
    }
    srand(time(NULL));
    int randIdx = rand() % profCount;
    strcpy(newStudent.advisorName, professors[profIndices[randIdx]].name);
    strcpy(newStudent.advisorID, professors[profIndices[randIdx]].id);

    // Assign random courses
    int numCourses = 6;
    int assignedCourses[numCourses];
    for (int i = 0; i < numCourses; i++) {
        int randCourseIdx;
        int duplicate;
        do {
            randCourseIdx = rand() % courseCount;
            duplicate = 0;
            for (int j = 0; j < i; j++) {
                if (assignedCourses[j] == randCourseIdx) {
                    duplicate = 1;
                    break;
                }
            }
        } while (duplicate);
        assignedCourses[i] = randCourseIdx;
        strcpy(newStudent.courses[i], courses[randCourseIdx].courseName);
        newStudent.grades[i] = 0.0;
    }

    students[studentCount] = newStudent;            //adding new student to the main students array
    studentCount++;
    saveStudents();
    saveStudentCourses();
    printf("Processing your request");
    for (int i = 0; i < 3; i++) {
        printf(".");
        fflush(stdout);
        sleep(1);}
    printf("\n");
    printf("\033[1;32mSuccess:\033[0m New student added successfully with ID: %s\n", newStudent.id);
}

void deleteStudent() {
    char targetID[MAX_STRING];
    printf("\n--- Delete Student ---\n");
    printf("Enter the Student ID to delete: ");
    fgets(targetID, MAX_STRING, stdin);
    targetID[strcspn(targetID, "\n")] = '\0'; // Remove newline character

    // Search for the student by ID
    int index = -1;
    for (int i = 0; i < studentCount; i++) {
        if (strcmp(students[i].id, targetID) == 0) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        printf("Student with ID '%s' not found.\n", targetID);
        return;
    }

    char confirmation[MAX_STRING];              //confirmation of deletion asking User
    printf("Are you sure you want to delete student '%s' (ID: %s)? (Y/N): ", students[index].name, students[index].id);
    fgets(confirmation, MAX_STRING, stdin);
    confirmation[strcspn(confirmation, "\n")] = '\0'; // Remove newline character

    if (strcasecmp(confirmation, "Y") != 0 && strcasecmp(confirmation, "YES") != 0) {
        printf("Deletion cancelled.\n");
        return;
    }
    for (int i = index; i < studentCount - 1; i++) {      // Shift students in the array to overwrite the deleted student
        students[i] = students[i + 1];
    }
    memset(&students[studentCount - 1], 0, sizeof(Student));        // Clear the last student entry 
    studentCount--;                                                 // Decrement student count
    saveStudents();
    saveStudentCourses();
    printf("Processing your request");
    for (int i = 0; i <3; i++) {
        printf(".");
        fflush(stdout);
        sleep(1);}
    printf("\n");
    printf("Student '%s' (ID: %s) has been successfully deleted.\n", students[index].name, targetID);
}

void viewInformationAdmin() {
    int choice;
    printf("1. View Student Information\n");
    printf("2. View Professor Information\n");
    printf("\033[1;33mEnter your choice:\033[0m ");
    scanf("%d", &choice);
    getchar();
    if (choice == 1) {
        char studentID[MAX_STRING];
        printf("Enter student ID: ");
        fgets(studentID, MAX_STRING, stdin);
        studentID[strcspn(studentID, "\n")] = '\0';
        for (int i = 0; i < studentCount; i++) {
            if (strcmp(students[i].id, studentID) == 0) {
                printf("ID: %s\nName: %s\nFaculty: %s\nAdvisor: %s\nEmail: %s\nPhone: %s\nDOB: %s\nGender: %c\nAddress: %s\nSemesters: %d\n",
                    students[i].id,
                    students[i].name,
                    students[i].faculty,
                    students[i].advisorName,
                    students[i].email,
                    students[i].phoneNumber,
                    students[i].dateOfBirth,
                    students[i].gender,
                    students[i].address,
                    students[i].semesters);
                displayCoursesAndGrades(&students[i]);
                double gpa = calculateGPA(&students[i]);
                printf("GPA: \033[1;34m%.2f\033[0m\n", gpa); // Highlight GPA in blue
                return;
            }
        }
        printf("Student not found.\n");
    } else if (choice == 2) {
        char profID[MAX_STRING];
        printf("Enter professor ID: ");
        fgets(profID, MAX_STRING, stdin);
        profID[strcspn(profID, "\n")] = '\0';
        for (int i = 0; i < professorCount; i++) {
            if (strcmp(professors[i].id, profID) == 0) {
                printf("ID: %s\nName: %s\nMajor: %s\nEmail: %s\nPhone: %s\n",
                    professors[i].id,
                    professors[i].name,
                    professors[i].major,
                    professors[i].email,
                    professors[i].phone);
                printf("Courses Teaching:\n");
                for (int j = 0; j < professors[i].courseCount; j++) {
                    char* courseName = getCourseNameByCode(professors[i].courses[j]);
                    if (courseName) {
                        printf("- %s\n", courseName);
                    } else {
                        printf("- %s (Course name not found)\n", professors[i].courses[j]);
                    }
                }
                return;
            }
        }
        printf("Professor not found.\n");
    } 
    else
    printf("\033[1;31mInvalid choice.\033[0m\n");
}

void changeInformationAdmin() {
    int choice;
    printf("1. Change Student Information\n");
    printf("2. Change Professor Information\n");
    printf("\033[1;33mEnter your choice:\033[0m ");
    scanf("%d", &choice);
    getchar();
    if (choice == 1) {
        char studentID[MAX_STRING];
        printf("Enter student ID: ");
        fgets(studentID, MAX_STRING, stdin);
        studentID[strcspn(studentID, "\n")] = '\0';
        for (int i = 0; i < studentCount; i++) {
            if (strcmp(students[i].id, studentID) == 0) {
                printf("1. Change Email\n2. Change Phone Number\n3. Change Address\n\033[1;33mEnter your choice:\033[0m ");
                int opt;
                scanf("%d", &opt);
                getchar();
                if (opt == 1) {
                    printf("Enter new email: ");
                    fgets(students[i].email, MAX_STRING, stdin);
                    students[i].email[strcspn(students[i].email, "\n")] = '\0';
                } else if (opt == 2) {
                    printf("Enter new phone number: ");
                    fgets(students[i].phoneNumber, MAX_STRING, stdin);
                    students[i].phoneNumber[strcspn(students[i].phoneNumber, "\n")] = '\0';
                } else if (opt == 3) {
                    printf("Enter new address: ");
                    fgets(students[i].address, MAX_STRING, stdin);
                    students[i].address[strcspn(students[i].address, "\n")] = '\0';
                } else {
                    printf("\033[1;31mInvalid option.\033[0m\n");
                    return;
                }
                saveStudents();
                printf("\033[1;32mInformation updated.\033[0m\n");
                return;
            }
        }
        printf("\033[1;31mStudent not found.\033[0m\n");
    } else if (choice == 2) {
        char profID[MAX_STRING];
        printf("\033[1;33mEnter professor ID:\033[0m ");
        fgets(profID, MAX_STRING, stdin);
        profID[strcspn(profID, "\n")] = '\0';
        for (int i = 0; i < professorCount; i++) {
            if (strcmp(professors[i].id, profID) == 0) {
                printf("1. Change Email\n2. Change Phone Number\n\033[1;33mEnter your choice:\033[0m ");
                int opt;
                scanf("%d", &opt);
                getchar();
                if (opt == 1) {
                    printf("\033[1;33mEnter new email:\033[0m "); 
                    fgets(professors[i].email, MAX_STRING, stdin);
                    professors[i].email[strcspn(professors[i].email, "\n")] = '\0';
                } else if (opt == 2) {
                    printf("\033[1;33mEnter new phone number:\033[0m ");
                    fgets(professors[i].phone, MAX_STRING, stdin);
                    professors[i].phone[strcspn(professors[i].phone, "\n")] = '\0';
                } else {
                    printf("\033[1;31mInvalid option.\033[0m\n");
                    return;
                }
                // Save professors function needed
                saveProfessors();
                printf("\033[1;32mInformation updated.\033[0m\n"); 
                return;
            }
        }
        printf("\033[1;31mProfessor not found.\033[0m\n");
    }
    else
    printf("\033[1;31mInvalid choice.\033[0m\n");
}

void encryptData() {
    int choice;
    printf("1. Encrypt Passwords\n2. Encrypt Addresses\n3. Encrypt Phone Numbers\n4. Encrypt Emails\n\033[1;33mEnter your choice:\033[0m ");
    if (scanf("%d", &choice) != 1) {
        printf("\033[1;31mInvalid input.\033[0m\n");
        while (getchar() != '\n'); // Clear input buffer
        return;
    }
    getchar(); // Consume newline

    int key;
    for (int i = 0; i < studentCount; i++) {
        int idLength = strlen(students[i].id);
        // Calculate key as the sum of the last two digits
        int digit1 = students[i].id[idLength - 1] - '0';
        int digit2 = students[i].id[idLength - 2] - '0';
        key = digit1 + digit2;

        if (choice == 1) {
            encrypt(students[i].password, key);
        } else if (choice == 2) {
            encrypt(students[i].address, key);
        } else if (choice == 3) {
            encrypt(students[i].phoneNumber, key);
        } else if (choice == 4) {
            encrypt(students[i].email, key);
        } else {
            printf("\033[1;31mInvalid choice.\033[0m\n");
            return;
        }
    }
    saveStudents();
    printf("\033[1;32mData encrypted.\033[0m\n"); 
}

void decryptData() {
    int choice;
    printf("1. Decrypt Passwords\n2. Decrypt Addresses\n3. Decrypt Phone Numbers\n4. Decrypt Emails\n\033[1;33mEnter your choice:\033[0m ");
    if (scanf("%d", &choice) != 1) {
        printf("\033[1;31mInvalid input.\033[0m\n");
        while (getchar() != '\n'); // Clear input buffer
        return;
    }
    getchar(); // Consume newline

    int key;
    for (int i = 0; i < studentCount; i++) {
        int idLength = strlen(students[i].id);
        // Calculate key as the sum of the last two digits
        int digit1 = students[i].id[idLength - 1] - '0';
        int digit2 = students[i].id[idLength - 2] - '0';
        key = digit1 + digit2;

        if (choice == 1) {
            decrypt(students[i].password, key);
        } else if (choice == 2) {
            decrypt(students[i].address, key);
        } else if (choice == 3) {
            decrypt(students[i].phoneNumber, key);
        } else if (choice == 4) {
            decrypt(students[i].email, key);
        } else {
            printf("\033[1;31mInvalid choice.\033[0m\n");
            return;
        }
    }
    saveStudents();
    printf("\033[1;32mData encrypted.\033[0m\n"); 
}

void changeProfessorInfo(Professor *prof) {
    int choice;
    printf("1. Change Email\n2. Change Phone Number\n\033[1;33mEnter your choice:\033[0m ");
    scanf("%d", &choice);
    getchar();
    if (choice == 1) {
        printf("\033[1;33mEnter new email:\033[0m "); 
        fgets(prof->email, MAX_STRING, stdin);
        prof->email[strcspn(prof->email, "\n")] = '\0';
    } else if (choice == 2) {
        printf("\033[1;33mEnter new phone number:\033[0m ");
        fgets(prof->phone, MAX_STRING, stdin);
        prof->phone[strcspn(prof->phone, "\n")] = '\0';
    } else {
        printf("\033[1;31mInvalid choice.\033[0m\n");
        return;
    }
    saveProfessors();       // Save the updated professor info
    printf("\033[1;32mInformation updated.\033[0m\n"); 
}

void viewStudentsProfessor(Professor *prof) {
    int found = 0;
    for (int i = 0; i < studentCount; i++) {
        if (strcmp(students[i].advisorID, prof->id) == 0) {
            found = 1;
            printf("\nID: %s, Name: %s\n", students[i].id, students[i].name);
            
            // Display Courses and Grades
            displayCoursesAndGrades(&students[i]);

            // Calculate and display GPA
            double gpa = calculateGPA(&students[i]);
            printf("GPA: %.2f\n", gpa); 
        }
    }
    if (!found) {
        printf("No students found under your advisement.\n");
    }
}

void generateStudentReport() {
    char studentID[MAX_STRING];
    printf("Enter student ID: ");
    fgets(studentID, MAX_STRING, stdin);
    studentID[strcspn(studentID, "\n")] = '\0';
    for (int i = 0; i < studentCount; i++) {
        if (strcmp(students[i].id, studentID) == 0) {
            char filename[MAX_STRING];
            sprintf(filename, "%s_report.txt", studentID);
            FILE *fp = fopen(filename, "w");
            if (!fp) {
                perror("File opening failed");
                printf("Error creating report file.\n");
                return;
            }
            fprintf(fp, "ID: %s\nName: %s\nFaculty: %s\nAdvisor: %s\nEmail: %s\nPhone: %s\nDOB: %s\nGender: %c\nAddress: %s\nSemesters: %d\n",
                    students[i].id,
                    students[i].name,
                    students[i].faculty,
                    students[i].advisorName,
                    students[i].email,
                    students[i].phoneNumber,
                    students[i].dateOfBirth,
                    students[i].gender,
                    students[i].address,
                    students[i].semesters);
            fclose(fp);
            printf("Report generated: %s\n", filename);
            return;
        }
    }
    printf("Student not found.\n");
}

void filterStudentsProfessor() {
    int choice;
    printf("Filter by:\n");
    printf("1. Department\n");
    printf("2. Scholarship Amount\n");
    printf("3. Country\n");
    printf("4. Semester\n");
    printf("\033[1;33mEnter your choice:\033[0m ");
    scanf("%d", &choice);
    getchar(); // Consume newline

    if (choice == 1) {
        char dept[MAX_STRING];
        printf("Enter department: ");
        fgets(dept, MAX_STRING, stdin);
        dept[strcspn(dept, "\n")] = '\0';
        printf("\n--- Students in Department: %s ---\n", dept);
        int found = 0;
        for (int i = 0; i < studentCount; i++) {
            if (strcmp(students[i].faculty, dept) == 0) {
                printf("ID: %s, Name: %s\n", students[i].id, students[i].name);
                found++;
            }
        }
        if (found == 0) {
            printf("\033[1;31mNo students found in the specified department.\033[0m\n");
        }
    } 
    else if (choice == 2) {
        int scholarship;
        printf("\033[1;33mEnter scholarship amount (percentage, e.g., 100 or 50):\033[0m ");
        scanf("%d", &scholarship);
        getchar(); // Consume newline
        if (scholarship < 0 || scholarship > 100) {
            printf("\033[1;31mInvalid scholarship amount. Please enter a value between 0 and 100.\033[0m\n");
            return;
        }
        printf("\n--- Students with %d%% Scholarship ---\n", scholarship);
        int found = 0;
        for (int i = 0; i < studentCount; i++) {
            if (students[i].scholarshipAmount == scholarship) {
                printf("ID: %s, Name: %s\n", students[i].id, students[i].name);
                found++;
            }
        }
        if (found == 0) {
            printf("\033[1;31mNo students found with the specified scholarship amount.\033[0m\n");
        }
    } 
    else if (choice == 3) {
        char country[MAX_STRING];
        printf("\033[1;33mEnter country:\033[0m ");
        fgets(country, MAX_STRING, stdin);
        country[strcspn(country, "\n")] = '\0';
        printf("\n--- Students from Country: %s ---\n", country);
        int found = 0;
        for (int i = 0; i < studentCount; i++) {
            if (strcmp(students[i].nationality, country) == 0) {
                printf("ID: %s, Name: %s\n", students[i].id, students[i].name);
                found++;
            }
        }
        if (found == 0) {
            printf("\033[1;31mNo students found from the specified country.\033[0m\n");
        }
    } 
    else if (choice == 4) {
        int sem;
        printf("\033[1;33mEnter semester:\033[0m ");
        scanf("%d", &sem);
        getchar(); // Consume newline
        if (sem < 1) {
            printf("\033[1;31mInvalid semester number. Please enter a positive integer.\033[0m\n");
            return;
        }
        printf("\n--- Students in Semester: %d ---\n", sem);
        int found = 0;
        for (int i = 0; i < studentCount; i++) {
            if (students[i].semesters == sem) {
                printf("ID: %s, Name: %s\n", students[i].id, students[i].name);
                found++;
            }
        }
        if (found == 0) {
            printf("\033[1;31mNo students found in the specified semester.\033[0m\n");
        }
    } 
    else {
        printf("\033[1;31mInvalid choice. Please select a valid option.\033[0m\n");
    }
}

void viewStudentInfo(Student *stud) {
    printf("\033[1;33mID:\033[0m %s\n", stud->id);
    printf("\033[1;33mName:\033[0m %s\n", stud->name);
    printf("\033[1;33mFaculty:\033[0m %s\n", stud->faculty);
    printf("\033[1;33mAdvisor:\033[0m %s\n", stud->advisorName);
    printf("\033[1;33mEmail:\033[0m %s\n", stud->email);
    printf("\033[1;33mPhone:\033[0m %s\n", stud->phoneNumber);
    printf("\033[1;33mDOB:\033[0m %s\n", stud->dateOfBirth);
    printf("\033[1;33mGender:\033[0m %c\n", stud->gender);
    printf("\033[1;33mAddress:\033[0m %s\n", stud->address);
    printf("\033[1;33mSemesters:\033[0m %d\n", stud->semesters);
    printf("\033[1;33mScholarship Amount:\033[0m %d%%\n", stud->scholarshipAmount);
    printf("\033[1;33mClubs Attending:\033[0m %s\n", stud->clubsAttending);
    printf("\033[1;33mNationality:\033[0m %s\n", stud->nationality);
    // Display Courses and Grades
    displayCoursesAndGrades(stud);
    // Calculate and display GPA
    double gpa = calculateGPA(stud);
    printf("GPA: %.2f\n", gpa);
}

void changeStudentInfo(Student *stud) {
    int choice;
    printf("1. Change Email\n2. Change Phone Number\n\033[1;33mEnter your choice:\033[0m ");
    scanf("%d", &choice);
    getchar();
    if (choice == 1) {
        printf("\033[1;33mEnter new email:\033[0m ");
        fgets(stud->email, MAX_STRING, stdin);
        stud->email[strcspn(stud->email, "\n")] = '\0';
    } else if (choice == 2) {
        printf("\033[1;33mEnter new phone number:\033[0m ");
        fgets(stud->phoneNumber, MAX_STRING, stdin);
        stud->phoneNumber[strcspn(stud->phoneNumber, "\n")] = '\0';
    } else {
        printf("\033[1;31mInvalid choice.\033[0m\n");
        return;
    }
    saveStudents();
    printf("\033[1;32mInformation updated.\033[0m\n");
}

void putGrade(Professor *prof) {
    // Display numbered list of courses the professor is teaching
    printf("\n--- Put Grade ---\n");
    printf("\033[1;33mCourses you are teaching:\033[0m\n\n");
    for (int i = 0; i < prof->courseCount; i++) {
        char* courseName = getCourseNameByCode(prof->courses[i]);
        if (courseName) {
            printf("%d. %s\n", i+1, courseName);
        } else {
            printf("%d. %s \033[1;31m(Course name not found)\033[0m\n", i+1, prof->courses[i]);
        }
    }
    if (prof->courseCount == 0) {
        printf("\033[1;31mYou are not teaching any courses.\033[0m\n");
        return;
    }
    printf("\033[1;33mEnter the number of the course to put grades for:\033[0m ");
    int courseChoice;
    scanf("%d", &courseChoice);
    getchar(); // consume newline

    if (courseChoice < 1 || courseChoice > prof->courseCount) {
        printf("\033[1;31mInvalid choice.\033[0m\n");
        return;
    }
    char *selectedCourseCode = prof->courses[courseChoice - 1];
    char *selectedCourseName = getCourseNameByCode(selectedCourseCode);

    if (!selectedCourseName) {
        printf("\033[1;31mCourse name not found for course code\033[0m %s.\n", selectedCourseCode);
        return;
    }
    // Now, find all students taking that course
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
        printf("\033[1;31mNo students are taking this course.\033[0m\n");
        return;
    }
    printf("Students taking %s:\n", selectedCourseName);
    for (int i = 0; i < studentsInCourseCount; i++) {
        printf("%d. %s (%s)\n", i+1, studentsInCourse[i]->name, studentsInCourse[i]->id);
    }
    printf("\033[1;31mEnter the number of the student to put grade for:\033[0m ");
    int studentChoice;
    scanf("%d", &studentChoice);
    getchar(); // consume newline

    if (studentChoice < 1 || studentChoice > studentsInCourseCount) {
        printf("\033[1;31mInvalid choice.\033[0m\n");
        return;
    }
    Student *selectedStudent = studentsInCourse[studentChoice - 1];
    // Now, ask for the grade
    printf("\033[1;31mEnter the grade (A+, A0, B+, B0, C+, C0, D+, D0):\033[0m ");
    char gradeInput[MAX_STRING];
    fgets(gradeInput, MAX_STRING, stdin);
    gradeInput[strcspn(gradeInput, "\n")] = '\0'; // remove newline

    double gradeValue;
    if (strcmp(gradeInput, "A+") == 0) {
        gradeValue = 4.5;
    } else if (strcmp(gradeInput, "A0") == 0) {
        gradeValue = 4.0;
    } else if (strcmp(gradeInput, "B+") == 0) {
        gradeValue = 3.5;
    } else if (strcmp(gradeInput, "B0") == 0) {
        gradeValue = 3.0;
    } else if (strcmp(gradeInput, "C+") == 0) {
        gradeValue = 2.5;
    } else if (strcmp(gradeInput, "C0") == 0) {
        gradeValue = 2.0;
    } else if (strcmp(gradeInput, "D+") == 0) {
        gradeValue = 1.5;
    } else if (strcmp(gradeInput, "D0") == 0) {
        gradeValue = 1.0;
    } else {
        printf("\033[1;31mInvalid grade input.\033[0m\n");
        return;
    }
    // Update the student's grade for the selected course
    int courseIndex = -1;
    for (int i = 0; i < 6; i++) {
        if (strcmp(selectedStudent->courses[i], selectedCourseName) == 0) {
            courseIndex = i;
            break;
        }
    }
    if (courseIndex == -1) {
        printf("\033[1;31mError: Student does not have this course.\033[0m\n");
        return;
    }
    selectedStudent->grades[courseIndex] = gradeValue;
    saveStudentCourses();
    printf("\033[1;32mGrade updated successfully.\033[0m\n");
}

int validateEmail(char *email) {
    return strstr(email, "@inha.edu") != NULL;
}
int validatePhoneNumber(char *phone) {
    return strlen(phone) == 11 && strncmp(phone, "010", 3) == 0;
}
int validatePassword(char *password) {
    if (strlen(password) != 10) {
        return 0; // Invalid length
    }
    int hasNumber = 0, hasSpecial = 0;
    for (int i = 0; password[i] != '\0'; i++) {
        if (isdigit(password[i])) hasNumber = 1;
        if (ispunct(password[i])) hasSpecial = 1;
    }
    return hasNumber && hasSpecial;
}
int validateName(char *name) {
    for (int i = 0; name[i]; i++) {
        if (!isalpha(name[i])) return 0;
    }
    return 1;
}
int validateBirthday(char *birthday) {
    return strlen(birthday) == 8;
}
int validateMajor(char *major) {
    return strcmp(major, "IBT") == 0 || strcmp(major, "ISE") == 0 || strcmp(major, "KLC") == 0;
}
void encrypt(char *text, int key) {
    for (int i = 0; text[i]; i++) {
        text[i] += key;
    }
}
void decrypt(char *text, int key) {
    for (int i = 0; text[i]; i++) {
        text[i] -= key;
    }
}
char* getCourseNameByCode(char* code) {
    for (int i = 0; i < courseCount; i++) {
        if (strcmp(courses[i].courseCode, code) == 0) {
            return courses[i].courseName;
        }
    }
    return NULL;
}
char* getCourseCodeByName(char* name) {
    for (int i = 0; i < courseCount; i++) {
        if (strcmp(courses[i].courseName, name) == 0) {
            return courses[i].courseCode;
        }
    }
    return NULL;
}

void getPassword(char *password, size_t max_len) {
    size_t index = 0;
    char ch;

#if defined(_WIN32) || defined(_WIN64)
    while ((ch = _getch()) != '\r' && index < max_len - 1) { // '\r' is carriage return in Windows
        if (ch == '\b') { // Handle backspace
            if (index > 0) {
                index--;
                printf("\b \b"); // Erase the last asterisk
            }
        }
        else if (ch == 3) { // Handle Ctrl+C
            printf("\n");
            exit(0);
        }
        else if (ch >= 32 && ch <= 126) { // Printable characters
            password[index++] = ch;
            printf("*");
        }
    }
#else
    struct termios oldt, newt;
    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    // Disable echo and canonical mode
    newt.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while ((ch = getchar()) != '\n' && ch != EOF && index < max_len - 1) {
        if (ch == 127 || ch == '\b') { // Handle backspace (127 for most Unix systems)
            if (index > 0) {
                index--;
                printf("\b \b"); // Erase the last asterisk
            }
        }
        else if (ch == 3) { // Handle Ctrl+C
            printf("\n");
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
            exit(0);
        }
        else if (ch >= 32 && ch <= 126) { // Printable characters
            password[index++] = ch;
            printf("*");
        }
    }

    // Restore terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    password[index] = '\0'; // Null-terminate the password string
    printf("\n"); // Move to the next line after password input
}

void displayWelcomeMessage() {
    printf("\t\t\t\t\t\t***********************************************\n");
    printf("\t\t\t\t\t\t*                                             *\n");
    printf("\t\t\t\t\t\t*      Welcome to the Student Management      *\n");
    printf("\t\t\t\t\t\t*                    System                   *\n");
    printf("\t\t\t\t\t\t*                                             *\n");
    printf("\t\t\t\t\t\t***********************************************\n");
    printf("\t\t\t\t\t\t*                                             *\n");
    printf("\t\t\t\t\t\t*    Manage your students' data efficiently   *\n");
    printf("\t\t\t\t\t\t*          and effortlessly with ease!        *\n");
    printf("\t\t\t\t\t\t*                                             *\n");
    printf("\t\t\t\t\t\t*      ----------------------------------     *\n");
    printf("\t\t\t\t\t\t*       Designed for Simplicity & Speed       *\n");
    printf("\t\t\t\t\t\t*      ----------------------------------     *\n");
    printf("\t\t\t\t\t\t*                                             *\n");
    printf("\t\t\t\t\t\t***********************************************\n");
    printf("\n\n");
    printf("\033[0;34mLoading the system, please wait\033[0m");
    for (int i = 0; i < 3; i++) {
        printf("\033[0;34m.\033[0m");
        fflush(stdout);
        sleep(1);}
    printf("\n\n");
}
