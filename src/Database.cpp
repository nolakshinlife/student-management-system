#include "Database.h"
#include <iostream>

// ------------------------------------------------------------
// Constructor
// ------------------------------------------------------------
// We initialise db to nullptr so isOpen() is correctly false
// until open() actually succeeds. dbPath is stored for later use
// inside open().
Database::Database(const std::string& path)
    : db(nullptr), dbPath(path) {
}

// ------------------------------------------------------------
// Destructor
// ------------------------------------------------------------
// RAII idea (Resource Acquisition Is Initialization): whoever
// creates a Database object doesn't have to remember to close
// it — the destructor guarantees cleanup automatically when the
// object goes out of scope.
Database::~Database() {
    close();
}

// ------------------------------------------------------------
// open()
// ------------------------------------------------------------
bool Database::open() {
    // sqlite3_open() creates the file if it doesn't exist yet,
    // and connects to it if it does. It returns SQLITE_OK (0)
    // on success.
    int result = sqlite3_open(dbPath.c_str(), &db);

    if (result != SQLITE_OK) {
        // Even on failure, sqlite3_open() may allocate a db handle
        // just to hold the error message, so we still read it
        // before treating the connection as unusable.
        std::cerr << "Error opening database: "
                  << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
        return false;
    }

    std::cout << "Database opened successfully at: " << dbPath << std::endl;
    return true;
}

// ------------------------------------------------------------
// close()
// ------------------------------------------------------------
void Database::close() {
    if (db != nullptr) {
        sqlite3_close(db);
        db = nullptr; // prevents double-close and makes isOpen() accurate
    }
}

// ------------------------------------------------------------
// isOpen()
// ------------------------------------------------------------
bool Database::isOpen() const {
    return db != nullptr;
}

// ------------------------------------------------------------
// executeSQL()
// ------------------------------------------------------------
// A small private helper used internally by this class whenever
// we need to run a statement that doesn't return rows of data.
// sqlite3_exec() is the simplest way to do this: give it a SQL
// string and it runs it in one call.
bool Database::executeSQL(const std::string& sql) {
    char* errMsg = nullptr;

    int result = sqlite3_exec(
        db,               // open connection
        sql.c_str(),      // SQL text to run
        nullptr,          // callback for each result row (not needed here)
        nullptr,          // argument passed to the callback (not needed here)
        &errMsg           // where SQLite writes an error message, if any
    );

    if (result != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg); // errMsg is allocated by SQLite; we must free it
        return false;
    }
    return true;
}

// ------------------------------------------------------------
// createStudentsTable()
// ------------------------------------------------------------
bool Database::createStudentsTable() {
    const std::string sql =
        "CREATE TABLE IF NOT EXISTS students ("
        "student_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name       TEXT NOT NULL, "
        "age        INTEGER NOT NULL CHECK(age > 0), "
        "course     TEXT NOT NULL, "
        "semester   INTEGER NOT NULL CHECK(semester BETWEEN 1 AND 8), "
        "marks      REAL NOT NULL CHECK(marks BETWEEN 0 AND 100), "
        "grade      TEXT, "
        "phone      TEXT, "
        "email      TEXT"
        ");";

    if (executeSQL(sql)) {
        std::cout << "Table 'students' is ready." << std::endl;
        return true;
    }
    return false;
}

// ============================================================
// extractStudentFromRow()
// ------------------------------------------------------------
// Reads all 9 columns of the CURRENT row (after a successful
// sqlite3_step() that returned SQLITE_ROW) and builds a Student.
// Column indices are 0-based and match the CREATE TABLE order:
// student_id, name, age, course, semester, marks, grade, phone, email.
// ============================================================
Student Database::extractStudentFromRow(sqlite3_stmt* stmt) {
    int id           = sqlite3_column_int(stmt, 0);
    // sqlite3_column_text returns an "unsigned char*"; we cast it
    // to "const char*" so it can build a std::string normally.
    std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    int age          = sqlite3_column_int(stmt, 2);
    std::string course = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    int semester     = sqlite3_column_int(stmt, 4);
    double marks     = sqlite3_column_double(stmt, 5);

    // grade/phone/email can be NULL in the database (we allow that
    // in the schema). sqlite3_column_text returns nullptr for NULL,
    // and building a std::string from nullptr would crash — so we
    // check first and substitute an empty string.
    const unsigned char* gradeText = sqlite3_column_text(stmt, 6);
    const unsigned char* phoneText = sqlite3_column_text(stmt, 7);
    const unsigned char* emailText = sqlite3_column_text(stmt, 8);

    std::string grade = gradeText ? reinterpret_cast<const char*>(gradeText) : "";
    std::string phone = phoneText ? reinterpret_cast<const char*>(phoneText) : "";
    std::string email = emailText ? reinterpret_cast<const char*>(emailText) : "";

    return Student(id, name, age, course, semester, marks, grade, phone, email);
}

// ============================================================
// insertStudent()
// ------------------------------------------------------------
// SQL used:
//   INSERT INTO students (name, age, course, semester, marks, grade, phone, email)
//   VALUES (?, ?, ?, ?, ?, ?, ?, ?);
//
// We don't list student_id — SQLite assigns it automatically
// because the column is INTEGER PRIMARY KEY AUTOINCREMENT.
// ============================================================
bool Database::insertStudent(Student& student) {
    const char* sql =
        "INSERT INTO students (name, age, course, semester, marks, grade, phone, email) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt = nullptr;

    // Step 1: "compile" the SQL text into a statement object.
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare insert statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Step 2: bind each "?" placeholder to a real value, in order.
    // The last argument (SQLITE_TRANSIENT) tells SQLite to make its
    // own internal copy of the string, since our std::string could
    // be destroyed before the statement actually runs.
    sqlite3_bind_text(stmt, 1, student.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, student.getAge());
    sqlite3_bind_text(stmt, 3, student.getCourse().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, student.getSemester());
    sqlite3_bind_double(stmt, 5, student.getMarks());
    sqlite3_bind_text(stmt, 6, student.getGrade().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, student.getPhone().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, student.getEmail().c_str(), -1, SQLITE_TRANSIENT);

    // Step 3: execute. For an INSERT, sqlite3_step() runs once and
    // should return SQLITE_DONE (not SQLITE_ROW, since INSERT
    // doesn't return any rows).
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    if (!success) {
        std::cerr << "Insert failed: " << sqlite3_errmsg(db) << std::endl;
    } else {
        // sqlite3_last_insert_rowid() gives us the AUTOINCREMENT
        // value SQLite just assigned to student_id.
        int newId = static_cast<int>(sqlite3_last_insert_rowid(db));
        student.setId(newId);
    }

    // Step 4: always finalize (free) the statement — every
    // sqlite3_prepare_v2() must be matched with a sqlite3_finalize().
    sqlite3_finalize(stmt);
    return success;
}

// ============================================================
// getAllStudents()
// ------------------------------------------------------------
// SQL used:  SELECT * FROM students;
// No "?" placeholders needed here since we're not filtering.
// ============================================================
std::vector<Student> Database::getAllStudents() {
    std::vector<Student> students;
    const char* sql = "SELECT * FROM students;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare select statement: " << sqlite3_errmsg(db) << std::endl;
        return students; // empty vector
    }

    // sqlite3_step() returns SQLITE_ROW once per row that exists,
    // and SQLITE_DONE once there are no more rows — that's what
    // drives this loop.
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        students.push_back(extractStudentFromRow(stmt));
    }

    sqlite3_finalize(stmt);
    return students;
}

// ============================================================
// findStudentById()
// ------------------------------------------------------------
// SQL used:  SELECT * FROM students WHERE student_id = ?;
// ============================================================
Student Database::findStudentById(int id, bool& found) {
    const char* sql = "SELECT * FROM students WHERE student_id = ?;";
    sqlite3_stmt* stmt = nullptr;
    found = false;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare select-by-id statement: " << sqlite3_errmsg(db) << std::endl;
        return Student();
    }

    sqlite3_bind_int(stmt, 1, id);

    Student result; // default/empty until we know otherwise
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        result = extractStudentFromRow(stmt);
        found = true;
    }
    // If sqlite3_step() returned SQLITE_DONE instead of SQLITE_ROW,
    // it means zero rows matched — found stays false.

    sqlite3_finalize(stmt);
    return result;
}

// ============================================================
// findStudentsByName()
// ------------------------------------------------------------
// SQL used:  SELECT * FROM students WHERE name LIKE ?;
// The '%' wildcards are added in C++ (around namePart) before
// binding, so a search for "an" matches "Anita", "Sanjay", etc.
// ============================================================
std::vector<Student> Database::findStudentsByName(const std::string& namePart) {
    std::vector<Student> students;
    const char* sql = "SELECT * FROM students WHERE name LIKE ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare select-by-name statement: " << sqlite3_errmsg(db) << std::endl;
        return students;
    }

    std::string pattern = "%" + namePart + "%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        students.push_back(extractStudentFromRow(stmt));
    }

    sqlite3_finalize(stmt);
    return students;
}

// ============================================================
// updateStudent()
// ------------------------------------------------------------
// SQL used:
//   UPDATE students
//   SET name = ?, age = ?, course = ?, semester = ?, marks = ?,
//       grade = ?, phone = ?, email = ?
//   WHERE student_id = ?;
//
// student_id itself is never updated — it's the row's permanent
// identity, so it only ever appears in the WHERE clause.
// ============================================================
bool Database::updateStudent(const Student& student) {
    const char* sql =
        "UPDATE students SET name = ?, age = ?, course = ?, semester = ?, "
        "marks = ?, grade = ?, phone = ?, email = ? WHERE student_id = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare update statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, student.getName().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, student.getAge());
    sqlite3_bind_text(stmt, 3, student.getCourse().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, student.getSemester());
    sqlite3_bind_double(stmt, 5, student.getMarks());
    sqlite3_bind_text(stmt, 6, student.getGrade().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, student.getPhone().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, student.getEmail().c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 9, student.getId()); // WHERE student_id = ?

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success) {
        std::cerr << "Update failed: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return success;
}

// ============================================================
// deleteStudent()
// ------------------------------------------------------------
// SQL used:  DELETE FROM students WHERE student_id = ?;
// ============================================================
bool Database::deleteStudent(int id) {
    const char* sql = "DELETE FROM students WHERE student_id = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare delete statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_int(stmt, 1, id);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success) {
        std::cerr << "Delete failed: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return success;
}
