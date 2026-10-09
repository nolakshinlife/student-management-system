#include "Student.h"
#include <iostream>
#include <iomanip>

// ------------------------------------------------------------
// Default constructor
// ------------------------------------------------------------
// Every member is given a safe, neutral starting value so an
// "empty" Student never contains garbage data.
Student::Student()
    : studentId(0), name(""), age(0), course(""), semester(0),
      marks(0.0), grade(""), phone(""), email("") {
}

// ------------------------------------------------------------
// Full constructor (used when loading an existing row from the DB)
// ------------------------------------------------------------
Student::Student(int id, const std::string& name, int age, const std::string& course,
                  int semester, double marks, const std::string& grade,
                  const std::string& phone, const std::string& email)
    : studentId(id), name(name), age(age), course(course), semester(semester),
      marks(marks), grade(grade), phone(phone), email(email) {
}

// ------------------------------------------------------------
// "New student" constructor (used when creating a student that
// does not exist in the database yet — no id, no grade supplied)
// ------------------------------------------------------------
Student::Student(const std::string& name, int age, const std::string& course,
                  int semester, double marks, const std::string& phone,
                  const std::string& email)
    : studentId(0), name(name), age(age), course(course), semester(semester),
      marks(marks), grade(""), phone(phone), email(email) {
    // grade starts empty; calculateGrade() must be called before saving.
}

// --- Getters ---
int Student::getId() const { return studentId; }
std::string Student::getName() const { return name; }
int Student::getAge() const { return age; }
std::string Student::getCourse() const { return course; }
int Student::getSemester() const { return semester; }
double Student::getMarks() const { return marks; }
std::string Student::getGrade() const { return grade; }
std::string Student::getPhone() const { return phone; }
std::string Student::getEmail() const { return email; }

// --- Setters ---
void Student::setId(int id) { studentId = id; }
void Student::setName(const std::string& n) { name = n; }
void Student::setAge(int a) { age = a; }
void Student::setCourse(const std::string& c) { course = c; }
void Student::setSemester(int s) { semester = s; }
void Student::setMarks(double m) { marks = m; }
void Student::setPhone(const std::string& p) { phone = p; }
void Student::setEmail(const std::string& e) { email = e; }

// ------------------------------------------------------------
// calculateGrade()
// ------------------------------------------------------------
// Simple fixed thresholds — easy to explain and easy to change
// later if your college uses a different grading scale.
void Student::calculateGrade() {
    if (marks >= 90)      grade = "A";
    else if (marks >= 75) grade = "B";
    else if (marks >= 60) grade = "C";
    else if (marks >= 40) grade = "D";
    else                  grade = "F";
}

// ------------------------------------------------------------
// display()
// ------------------------------------------------------------
// Prints one neatly aligned row. std::left + std::setw() pad each
// field to a fixed width so multiple rows line up into columns.
void Student::display() const {
    std::cout << std::left
              << std::setw(5)  << studentId
              << std::setw(18) << name
              << std::setw(5)  << age
              << std::setw(10) << course
              << std::setw(10) << semester
              << std::setw(8)  << marks
              << std::setw(7)  << grade
              << std::setw(14) << phone
              << email
              << std::endl;
}
