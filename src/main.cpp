// ============================================================
// main.cpp
// ------------------------------------------------------------
// Responsible ONLY for:
//   1. Setting up the Database (open connection, create table)
//   2. Handing that Database to a StudentManager
//   3. Showing the menu and reading the user's choice
//   4. Calling the matching StudentManager method
//
// main.cpp never touches SQL directly, and never touches a
// Student's fields directly — it only calls StudentManager.
// ============================================================

#include <iostream>
#include "Database.h"
#include "StudentManager.h"

// Prints the menu options. Kept as its own function so main()
// stays short and readable.
void printMenu() {
    std::cout << "\n===== Student Management System =====\n";
    std::cout << "1. Add Student\n";
    std::cout << "2. View All Students\n";
    std::cout << "3. Search Student by ID\n";
    std::cout << "4. Search Student by Name\n";
    std::cout << "5. Update Student\n";
    std::cout << "6. Delete Student\n";
    std::cout << "7. Exit\n";
    std::cout << "Enter your choice: ";
}

int main() {
    // 1. Set up the database connection.
    // main() OWNS this object — it opens it, and it will close it
    // before the program ends. StudentManager only borrows it.
    Database db("db/students.db");

    if (!db.open()) {
        std::cerr << "Could not open the database. Exiting program." << std::endl;
        return 1;
    }

    if (!db.createStudentsTable()) {
        std::cerr << "Could not set up the students table. Exiting program." << std::endl;
        db.close();
        return 1;
    }

    // 2. Create the manager, giving it the database to work with.
    StudentManager manager(db);

    // 3. Menu loop.
    int choice = 0;
    bool running = true;

    while (running) {
        printMenu();
        std::cin >> choice;

        // Basic safety check for this stage: if the user types
        // something that isn't a number, std::cin enters a "fail"
        // state. We reset it here so the loop doesn't spin forever.
        // (Full input validation is a later stage — this is just
        // enough to keep the skeleton from crashing.)
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1:
                manager.addStudent();
                break;
            case 2:
                manager.viewAllStudents();
                break;
            case 3:
                manager.searchStudentById();
                break;
            case 4:
                manager.searchStudentByName();
                break;
            case 5:
                manager.updateStudent();
                break;
            case 6:
                manager.deleteStudent();
                break;
            case 7:
                std::cout << "Exiting. Goodbye!\n";
                running = false;
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    }

    // 4. Clean up. (Database's destructor would also close it
    // automatically, but closing explicitly here is clearer to read.)
    db.close();

    return 0;
}
