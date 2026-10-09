// ============================================================
// test_database.cpp
// ------------------------------------------------------------
// This is a TEMPORARY test file, not part of the final
// application. Its only job is to prove that the Database
// class can open a connection and create the students table.
// Later, when Student and StudentManager exist, this file will
// be replaced by main.cpp with the real menu.
//
// We also insert one raw test row and read it back using plain
// SQL here (sqlite3_exec with a callback), just to prove that
// data actually persists in the .db file. Proper insert/search/
// update/delete methods will be added to the Database class in
// the next stage — this is only a sanity check.
// ============================================================

#include "Database.h"
#include <sqlite3.h>
#include <iostream>

// A callback function sqlite3_exec() will call once per result row.
// This matches the exact signature SQLite expects.
static int printRowCallback(void* /*unused*/, int columnCount,
                             char** columnValues, char** columnNames) {
    for (int i = 0; i < columnCount; i++) {
        std::cout << columnNames[i] << " = "
                  << (columnValues[i] ? columnValues[i] : "NULL") << "  ";
    }
    std::cout << std::endl;
    return 0; // returning 0 tells SQLite "continue, no error"
}

int main() {
    // 1. Create and open the database
    Database db("db/students.db");

    if (!db.open()) {
        std::cerr << "Failed to open database. Exiting." << std::endl;
        return 1;
    }

    // 2. Create the students table
    if (!db.createStudentsTable()) {
        std::cerr << "Failed to create table. Exiting." << std::endl;
        return 1;
    }

    // 3. Insert one test row using raw SQL, just to prove writes work.
    //    (This bypasses the Database class's private executeSQL on
    //    purpose, using sqlite3_exec directly here in the test file,
    //    since we haven't built an insertStudent() method yet.)
    sqlite3* rawHandle = nullptr;
    if (sqlite3_open("db/students.db", &rawHandle) == SQLITE_OK) {
        const char* insertSQL =
            "INSERT INTO students (name, age, course, semester, marks, grade) "
            "VALUES ('Test Student', 20, 'BCA', 6, 78.5, 'B');";

        char* errMsg = nullptr;
        if (sqlite3_exec(rawHandle, insertSQL, nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "Insert failed: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        } else {
            std::cout << "Test row inserted." << std::endl;
        }

        // 4. Read back everything in the table to confirm it was saved.
        std::cout << "\n--- Contents of students table ---" << std::endl;
        const char* selectSQL = "SELECT * FROM students;";
        if (sqlite3_exec(rawHandle, selectSQL, printRowCallback, nullptr, &errMsg) != SQLITE_OK) {
            std::cerr << "Select failed: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }

        sqlite3_close(rawHandle);
    }

    // 5. Close the Database class's connection too
    db.close();

    std::cout << "\nDatabase layer test finished." << std::endl;
    return 0;
}
