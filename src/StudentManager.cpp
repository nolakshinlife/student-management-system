#include "StudentManager.h"
#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>

// ------------------------------------------------------------
// Small file-local helper functions (not class members).
// They exist purely to avoid repeating the same "read + check"
// loop in every method below. This is basic, not advanced —
// full validation (Validator class) is a later stage; these
// just stop the skeleton from crashing on bad input.
// ------------------------------------------------------------

// Reads an integer and keeps re-prompting until it's a valid
// number within [minVal, maxVal].
static int readValidatedInt(const std::string& prompt, int minVal, int maxVal) {
    int value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;

        if (std::cin.fail() || value < minVal || value > maxVal) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Enter a whole number between "
                      << minVal << " and " << maxVal << ".\n";
            continue;
        }

        // Consume the leftover newline so a later getline() doesn't
        // read an empty line by mistake.
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return value;
    }
}

// Same idea, for decimal marks.
static double readValidatedDouble(const std::string& prompt, double minVal, double maxVal) {
    double value;
    while (true) {
        std::cout << prompt;
        std::cin >> value;

        if (std::cin.fail() || value < minVal || value > maxVal) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Enter a number between "
                      << minVal << " and " << maxVal << ".\n";
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return value;
    }
}

// Reads a full line of text (used for name/course/phone/email).
static std::string readLine(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin, value);
    return value;
}

// Prints the column header row once, above a list of students.
static void printTableHeader() {
    std::cout << std::left
              << std::setw(5)  << "ID"
              << std::setw(18) << "Name"
              << std::setw(5)  << "Age"
              << std::setw(10) << "Course"
              << std::setw(10) << "Semester"
              << std::setw(8)  << "Marks"
              << std::setw(7)  << "Grade"
              << std::setw(14) << "Phone"
              << "Email"
              << std::endl;
}

// ------------------------------------------------------------
// Constructor
// ------------------------------------------------------------
StudentManager::StudentManager(Database& database)
    : db(database) {
}

// ------------------------------------------------------------
// addStudent()
// ------------------------------------------------------------
void StudentManager::addStudent() {
    std::cout << "\n--- Add Student ---\n";

    // Flush the leftover newline left behind by the menu's
    // "cin >> choice" in main.cpp, so the first getline() below
    // doesn't immediately read an empty line.
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string name    = readLine("Enter name: ");
    int age             = readValidatedInt("Enter age: ", 1, 100);
    std::string course  = readLine("Enter course: ");
    int semester        = readValidatedInt("Enter semester (1-8): ", 1, 8);
    double marks        = readValidatedDouble("Enter marks (0-100): ", 0, 100);
    std::string phone   = readLine("Enter phone (optional): ");
    std::string email   = readLine("Enter email (optional): ");

    // "New student" constructor: no id, no grade yet.
    Student student(name, age, course, semester, marks, phone, email);
    student.calculateGrade();

    if (db.insertStudent(student)) {
        // student.getId() now holds the ID the database assigned,
        // because insertStudent() writes it back via setId().
        std::cout << "Student added successfully with ID: " << student.getId() << std::endl;
    } else {
        std::cout << "Failed to add student. Please try again.\n";
    }
}

// ------------------------------------------------------------
// viewAllStudents()
// ------------------------------------------------------------
void StudentManager::viewAllStudents() {
    std::vector<Student> students = db.getAllStudents();

    if (students.empty()) {
        std::cout << "No students found.\n";
        return;
    }

    std::cout << "\n--- All Students (" << students.size() << ") ---\n";
    printTableHeader();
    for (const Student& s : students) {
        s.display();
    }
}

// ------------------------------------------------------------
// searchStudentById()
// ------------------------------------------------------------
void StudentManager::searchStudentById() {
    int id = readValidatedInt("Enter student ID to search: ", 1, std::numeric_limits<int>::max());

    bool found = false;
    Student student = db.findStudentById(id, found);

    if (found) {
        std::cout << "\n--- Student Found ---\n";
        printTableHeader();
        student.display();
    } else {
        std::cout << "No student found with ID " << id << ".\n";
    }
}

// ------------------------------------------------------------
// searchStudentByName()
// ------------------------------------------------------------
void StudentManager::searchStudentByName() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::string namePart = readLine("Enter name (or part of it) to search: ");

    std::vector<Student> results = db.findStudentsByName(namePart);

    if (results.empty()) {
        std::cout << "No matching students found.\n";
        return;
    }

    std::cout << "\n--- Matching Students (" << results.size() << ") ---\n";
    printTableHeader();
    for (const Student& s : results) {
        s.display();
    }
}

// ------------------------------------------------------------
// updateStudent()
// ------------------------------------------------------------
void StudentManager::updateStudent() {
    int id = readValidatedInt("Enter student ID to update: ", 1, std::numeric_limits<int>::max());

    bool found = false;
    Student student = db.findStudentById(id, found);

    if (!found) {
        std::cout << "No student found with ID " << id << ".\n";
        return;
    }

    std::cout << "Current details:\n";
    printTableHeader();
    student.display();
    std::cout << "\nEnter new details below (all fields required):\n";

    std::string name    = readLine("Enter name: ");
    int age             = readValidatedInt("Enter age: ", 1, 100);
    std::string course  = readLine("Enter course: ");
    int semester        = readValidatedInt("Enter semester (1-8): ", 1, 8);
    double marks        = readValidatedDouble("Enter marks (0-100): ", 0, 100);
    std::string phone   = readLine("Enter phone (optional): ");
    std::string email   = readLine("Enter email (optional): ");

    student.setName(name);
    student.setAge(age);
    student.setCourse(course);
    student.setSemester(semester);
    student.setMarks(marks);
    student.calculateGrade(); // recompute — marks may have changed
    student.setPhone(phone);
    student.setEmail(email);
    // studentId is untouched — it's the row's permanent identity.

    if (db.updateStudent(student)) {
        std::cout << "Student updated successfully.\n";
    } else {
        std::cout << "Failed to update student.\n";
    }
}

// ------------------------------------------------------------
// deleteStudent()
// ------------------------------------------------------------
void StudentManager::deleteStudent() {
    int id = readValidatedInt("Enter student ID to delete: ", 1, std::numeric_limits<int>::max());

    bool found = false;
    Student student = db.findStudentById(id, found);

    if (!found) {
        std::cout << "No student found with ID " << id << ".\n";
        return;
    }

    std::cout << "Student to delete:\n";
    printTableHeader();
    student.display();

    std::string confirm = readLine("\nAre you sure you want to delete this student? (Y/N): ");

    if (confirm == "Y" || confirm == "y") {
        if (db.deleteStudent(id)) {
            std::cout << "Student deleted successfully.\n";
        } else {
            std::cout << "Failed to delete student.\n";
        }
    } else {
        std::cout << "Delete cancelled.\n";
    }
}
