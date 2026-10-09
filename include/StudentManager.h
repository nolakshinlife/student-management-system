#ifndef STUDENT_MANAGER_H
#define STUDENT_MANAGER_H

#include "Database.h"

// ============================================================
// StudentManager class
// ------------------------------------------------------------
// This is the "controller" / business-logic layer. main.cpp
// calls these methods; these methods decide WHAT needs to
// happen (e.g. "compute grade before saving") and then ask
// Database to actually store/retrieve data.
//
// StudentManager does NOT know any SQL, and does NOT print
// menus. It sits strictly between the two.
// ============================================================
class StudentManager {
private:
    Database& db; // Reference to an ALREADY-OPEN Database object.
                   // We use a reference (not a copy) because there
                   // should only ever be one real connection, owned
                   // by main(); StudentManager just borrows it.

public:
    // Constructor takes the Database to work with. This pattern is
    // called "dependency injection" — StudentManager doesn't create
    // its own Database, main.cpp gives it one. This makes it easy to
    // test StudentManager independently later if you ever wanted to.
    explicit StudentManager(Database& database);

    // Each method below corresponds to one menu option.
    // STUBS for this stage — real prompting/validation logic
    // (reading input, calling Student's setters, calling
    // calculateGrade(), then db.insertStudent(), etc.) comes next.

    void addStudent();
    void viewAllStudents();
    void searchStudentById();
    void searchStudentByName();
    void updateStudent();
    void deleteStudent();
};

#endif // STUDENT_MANAGER_H
