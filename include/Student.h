#ifndef STUDENT_H
#define STUDENT_H

#include <string>

// ============================================================
// Student class
// ------------------------------------------------------------
// Represents ONE student record. This class does not know
// anything about menus or databases — it only holds data and
// provides behavior that operates on that data (encapsulation).
// ============================================================
class Student {
private:
    int studentId;      // Primary key from the database.
                         // 0 means "not yet saved / not yet assigned an ID".
    std::string name;
    int age;
    std::string course;
    int semester;
    double marks;
    std::string grade;   // Derived from marks, not entered by the user directly.
    std::string phone;   // Optional field.
    std::string email;   // Optional field.

public:
    // Default constructor: creates an "empty" student.
    // Useful when we need a placeholder before filling in real data,
    // e.g. Student s; then s.setName(...); etc.
    Student();

    // Full constructor: used when LOADING a student that already
    // exists in the database (so we already know its studentId
    // and its stored grade).
    Student(int id, const std::string& name, int age, const std::string& course,
            int semester, double marks, const std::string& grade,
            const std::string& phone, const std::string& email);

    // "New student" constructor: used when CREATING a student that
    // does NOT exist in the database yet. There is no id parameter,
    // because SQLite will assign the id automatically (AUTOINCREMENT).
    // There is also no grade parameter — grade will be computed by
    // calculateGrade(), not typed in by the user.
    Student(const std::string& name, int age, const std::string& course,
            int semester, double marks, const std::string& phone,
            const std::string& email);

    // --- Getters ---
    // Marked 'const' because reading a value should never modify
    // the object. This is a small but real detail interviewers ask about.
    int getId() const;
    std::string getName() const;
    int getAge() const;
    std::string getCourse() const;
    int getSemester() const;
    double getMarks() const;
    std::string getGrade() const;
    std::string getPhone() const;
    std::string getEmail() const;

    // --- Setters ---
    // Exist so outside code can never set an invalid value directly
    // on the private members. (Actual validation logic will be added
    // in a later stage, in the Validator helper.)
    void setId(int id);
    void setName(const std::string& name);
    void setAge(int age);
    void setCourse(const std::string& course);
    void setSemester(int semester);
    void setMarks(double marks);
    void setPhone(const std::string& phone);
    void setEmail(const std::string& email);

    // Computes 'grade' from 'marks' and stores it in the grade member.
    // Kept as a class method (not a free function) because grade is
    // fundamentally a property OF a student's marks.
    // STUB for now — real threshold logic comes in the next stage.
    void calculateGrade();

    // Prints this student's fields in a readable format.
    // STUB for now — real formatting comes in the next stage.
    void display() const;
};

#endif // STUDENT_H
