#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include "Student.h"

// ============================================================
// Database class
// ------------------------------------------------------------
// Purpose: This class is the ONLY part of the program that
// knows about SQLite. Every other class will talk to the
// database through this class's methods, never by calling
// sqlite3_* functions directly. This is "abstraction" in OOP:
// we hide the messy C API behind simple, readable methods.
// ============================================================
class Database {
private:
    sqlite3* db;        // Raw handle to the open SQLite connection.
                         // nullptr when the database is not open.
    std::string dbPath;  // File path of the .db file, e.g. "db/students.db"

    // Runs a SQL statement that does not return rows
    // (CREATE TABLE, INSERT, UPDATE, DELETE all use this later).
    // Kept private because outside classes should never run raw
    // SQL directly — they should call named methods like
    // createStudentsTable() instead.
    bool executeSQL(const std::string& sql);

    // Builds a Student object from the CURRENT row of an already
    // stepped prepared statement. Used by getAllStudents(),
    // findStudentById(), and findStudentsByName() so we don't repeat
    // the same "read each column" code three times.
    Student extractStudentFromRow(sqlite3_stmt* stmt);

public:
    // Constructor: just stores the file path, does NOT open the
    // connection yet. Opening is a separate, explicit step (see open()).
    explicit Database(const std::string& path);

    // Destructor: automatically closes the connection if it is
    // still open, so we never "leak" an open database handle
    // even if the caller forgets to call close().
    ~Database();

    // Opens the connection to the .db file (creates the file if
    // it does not already exist). Returns true on success.
    bool open();

    // Closes the connection. Safe to call even if already closed.
    void close();

    // Returns true if the connection is currently open.
    bool isOpen() const;

    // Creates the "students" table if it does not already exist.
    // Returns true on success.
    bool createStudentsTable();

    // ------------------------------------------------------------
    // CRUD methods — declared now so StudentManager has something
    // to call, but only STUBBED (not yet implemented). Real SQL
    // (INSERT/SELECT/UPDATE/DELETE with prepared statements) is
    // built in the next stage.
    // ------------------------------------------------------------

    // Inserts a new student row. Returns true on success.
    // Takes a NON-const reference because, after a successful insert,
    // this method writes the database-assigned ID back into the
    // student object via student.setId(...).
    bool insertStudent(Student& student);

    // Returns every student currently in the table.
    std::vector<Student> getAllStudents();

    // Returns the student with the given ID, if found.
    // 'found' is set to true/false so the caller can tell an
    // empty/default Student apart from "no such student".
    Student findStudentById(int id, bool& found);

    // Returns all students whose name matches (partial match, e.g. LIKE).
    std::vector<Student> findStudentsByName(const std::string& namePart);

    // Updates an existing student's row (matched by student.getId()).
    // Returns true on success.
    bool updateStudent(const Student& student);

    // Deletes the student with the given ID. Returns true on success.
    bool deleteStudent(int id);
};

#endif // DATABASE_H
