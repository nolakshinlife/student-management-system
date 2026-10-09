CREATE TABLE IF NOT EXISTS students (
    student_id INTEGER PRIMARY KEY AUTOINCREMENT,
    name       TEXT NOT NULL,
    age        INTEGER NOT NULL CHECK(age > 0),
    course     TEXT NOT NULL,
    semester   INTEGER NOT NULL CHECK(semester BETWEEN 1 AND 8),
    marks      REAL NOT NULL CHECK(marks BETWEEN 0 AND 100),
    grade      TEXT,
    phone      TEXT,
    email      TEXT
);