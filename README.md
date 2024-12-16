# Student Management System

This **Student Management System (SMS)** is a robust command-line application written in C that manages student, professor, and course information efficiently. It is designed with separate interfaces and functionalities for **Administrators**, **Professors**, and **Students**, ensuring that each user type has access to the tools and data they need.

**Note:** This project is closed-source in terms of contributions. **Contributions are not welcomed.** Any unsolicited pull requests, issues, or feature requests will be closed without consideration.

---

## Key Features

### Administrator (Admin) Portal
- **User Management**:  
  - Add new students with automated ID assignment and random advisor selection based on the student’s major.  
  - Delete students after confirmation, ensuring data is up-to-date and accurate.
- **Data Security Operations**:  
  - Encrypt or decrypt sensitive fields (e.g., passwords, addresses, phone numbers, emails) for enhanced data security.
- **Information Editing**:  
  - View and modify student or professor information, such as email, phone number, or address.
- **Data Validation**:  
  - Built-in validation for critical fields (e.g., email formats, phone numbers, and birthdays) to maintain data integrity.

### Professor Portal
- **Profile Updates**:  
  - Edit personal contact details (email and phone number) easily.
- **Student Advisement**:  
  - View all students under your advisement, including their course enrollments and academic performance.
- **Grading Tools**:  
  - Assign and update student grades with user-friendly prompts.  
- **Advanced Filtering and Reporting**:  
  - Filter students by department, scholarship amounts, nationality, or semester.
  - Generate text-based student reports (e.g., `12345_report.txt`) that can be archived or shared.
  
### Student Portal
- **Personal Dashboard**:  
  - View personal info (ID, name, advisor, email, phone, DOB, gender, address, nationality, clubs, scholarships, semester).
- **Academic Overview**:  
  - Review enrolled courses and view letter grades (A+, A0, B+, etc.).
  - Calculate and display GPA, providing immediate academic performance insight.
- **Edit Contact Info**:  
  - Update personal email and phone number while adhering to validation rules.

---

## Data Files and Storage

- **`students.txt`**:  
  Stores detailed student records, including personal and academic information.
  
- **`professors.txt`**:  
  Contains professor data, including their assigned courses and contact information.
  
- **`courses.txt`**:  
  A list of courses, each with a name and a unique code.

- **`student_classes.txt`**:  
  Maps student IDs to the courses they are enrolled in, along with their grades.

All files are expected to reside in the same directory as the executable.

---

## Security and Validation

- **Data Encryption/Decryption**:  
  Select fields (passwords, addresses, phone numbers, emails) can be encrypted or decrypted by the administrator. The encryption key is dynamically derived from student IDs, ensuring each student’s data can be secured individually.

- **Input Validation**:  
  Strict rules are enforced for:  
  - **Email**: Must contain `@inha.edu` domain.  
  - **Phone Number**: Must start with `010` and be exactly 11 digits long.  
  - **Passwords**: Must be exactly 10 characters, containing at least one digit and one special character.  
  - **Birthdays**: Must follow the `yyyymmdd` format.  
  - **Names and Majors**: Only predefined majors (IBT, ISE, KLC) and alphabetic-only names are allowed.

---

## User Interface and Experience (UI/UX)

- **Text-Based Menus**:  
  Clean and well-organized command-line menus guide you through the system’s functionalities.
  
- **Color Coding (If Supported)**:  
  ANSI escape codes can be used to enhance the visual appeal of menus and messages. For example, errors can be displayed in **red**, successes in **green**, and headings in **bold blue**. On Unix-like systems and modern Windows terminals, color formatting will improve readability.

- **Feedback Messages**:  
  User actions (e.g., adding a student, deleting a record) are acknowledged with clear success or error messages. Invalid inputs trigger explanatory prompts to help users correct their input.

- **Progress Indicators**:  
  Loading messages and simple progress indicators (`...`) can be added to show that the system is processing data.

---

## How to Build and Run

1. **Prerequisites**:  
   Ensure you have a C compiler (e.g., `gcc`) and the necessary header files available on your system.  
   
2. **Compilation**:  
   ```bash
   gcc main.c -o student_management
   ```
   
3. **Execution**:  
   ```bash
   ./student_management
   ```
   If you’re using Windows, and if colors do not appear, consider enabling ANSI support or running the program in Windows Terminal or Git Bash.

4. **File Setup**:  
   Ensure the required data files (`students.txt`, `professors.txt`, `courses.txt`, `student_classes.txt`) are present in the same directory. If the files are not present, the program may display warnings or create empty ones.

---

## Example Workflows

- **Admin Logging In**:  
  Use the admin credentials (`admin`, `admin123`) to access admin menu options like encrypting fields or adding a new student.

- **Professor Grading**:  
  After logging in as a professor, select the “Put Grade” option, choose a course, and assign a letter grade (A+, A0, B+, etc.) to a student’s record.

- **Student Checking GPA**:  
  A student logs in, views their course enrollments and grades, and sees an automatically calculated GPA displayed on the screen.

---

## Limitations and Notes

- **Command-Line Based**:  
  This system does not have a graphical user interface. All interactions occur in a terminal/console environment.
  
- **Static Array Sizes**:  
  Due to the project’s scope and simplicity, maximum capacities for students, professors, and courses are defined by macros. Consider increasing these if your use case expands.
