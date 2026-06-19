#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstring>

using namespace std;

// Class representing the Student entity (Encapsulation)
class Student {
private:
    int id;
    char name[50];
    int age;
    char course[50];

public:
    // Default constructor
    Student() {
        id = 0;
        age = 0;
        strcpy(name, "");
        strcpy(course, "");
    }

    // Parameterized constructor
    Student(int sId, const char* sName, int sAge, const char* sCourse) {
        id = sId;
        age = sAge;
        strncpy(name, sName, 49);
        name[49] = '\0'; // Ensure null-termination
        strncpy(course, sCourse, 49);
        course[49] = '\0';
    }

    // Getters
    int getId() const { return id; }
    const char* getName() const { return name; }
    int getAge() const { return age; }
    const char* getCourse() const { return course; }

    // Setters for modification
    void setName(const char* sName) {
        strncpy(name, sName, 49);
        name[49] = '\0';
    }
    void setAge(int sAge) { age = sAge; }
    void setCourse(const char* sCourse) {
        strncpy(course, sCourse, 49);
        course[49] = '\0';
    }
};

// Class handling all file database operations
class StudentManager {
private:
    string fileName;

public:
    StudentManager(string dbName) {
        fileName = dbName;
    }

    // 1. CREATE
    void addStudent() {
        int id, age;
        char name[50], course[50];

        ofstream outFile(fileName, ios::binary | ios::app);
        if (!outFile) {
            cout << "Error: Could not open database file.\n";
            return;
        }

        cout << "\n--- Add New Student Record ---\n";
        cout << "Enter Student ID: ";
        cin >> id;
        cin.ignore(); 
        
        cout << "Enter Name: ";
        cin.getline(name, 50);
        
        cout << "Enter Age: ";
        cin >> age;
        cin.ignore();
        
        cout << "Enter Course: ";
        cin.getline(course, 50);

        // Create object and write its raw memory to file
        Student s(id, name, age, course);
        outFile.write(reinterpret_cast<char*>(&s), sizeof(Student));
        outFile.close();

        cout << "Record saved successfully.\n";
    }

    // 2. READ
    void displayAllStudents() {
        Student s;
        ifstream inFile(fileName, ios::binary);

        if (!inFile) {
            cout << "\nNo records found. Database file might be empty.\n";
            return;
        }

        cout << "\n==================== STUDENT RECORDS ====================\n";
        cout << left << setw(10) << "ID" << setw(25) << "Name" << setw(10) << "Age" << setw(20) << "Course" << endl;
        cout << "---------------------------------------------------------\n";

        while (inFile.read(reinterpret_cast<char*>(&s), sizeof(Student))) {
            cout << left << setw(10) << s.getId() 
                 << setw(25) << s.getName() 
                 << setw(10) << s.getAge() 
                 << setw(20) << s.getCourse() << endl;
        }
        cout << "=========================================================\n";
        inFile.close();
    }

    // 3. UPDATE
    void updateStudent() {
        Student s;
        fstream file(fileName, ios::binary | ios::in | ios::out);
        int targetId;
        bool found = false;

        if (!file) {
            cout << "\nError opening file or no records exist yet.\n";
            return;
        }

        cout << "\nEnter Student ID to modify: ";
        cin >> targetId;

        while (file.read(reinterpret_cast<char*>(&s), sizeof(Student))) {
            if (s.getId() == targetId) {
                found = true;
                cout << "\nRecord Found -> Name: " << s.getName() << ", Age: " << s.getAge() << ", Course: " << s.getCourse() << "\n";

                char newName[50], newCourse[50];
                int newAge;

                cin.ignore();
                cout << "Enter New Name: ";
                cin.getline(newName, 50);
                cout << "Enter New Age: ";
                cin >> newAge;
                cin.ignore();
                cout << "Enter New Course: ";
                cin.getline(newCourse, 50);

                // Update data using setters
                s.setName(newName);
                s.setAge(newAge);
                s.setCourse(newCourse);

                // Move cursor back to rewrite the object
                int offset = -1 * static_cast<int>(sizeof(Student));
                file.seekp(offset, ios::cur);
                
                file.write(reinterpret_cast<char*>(&s), sizeof(Student));
                cout << "Record updated successfully.\n";
                break;
            }
        }

        if (!found) {
            cout << "Student with ID " << targetId << " not found.\n";
        }
        file.close();
    }

    // 4. DELETE
    void deleteStudent() {
        Student s;
        ifstream inFile(fileName, ios::binary);
        int targetId;
        bool found = false;

        if (!inFile) {
            cout << "\nNo records available to delete.\n";
            return;
        }

        cout << "\nEnter Student ID to delete: ";
        cin >> targetId;

        ofstream tempFile("temp.dat", ios::binary);

        while (inFile.read(reinterpret_cast<char*>(&s), sizeof(Student))) {
            if (s.getId() == targetId) {
                found = true; 
            } else {
                tempFile.write(reinterpret_cast<char*>(&s), sizeof(Student));
            }
        }

        inFile.close();
        tempFile.close();

        remove(fileName.c_str());
        rename("temp.dat", fileName.c_str());

        if (found) {
            cout << "Record deleted successfully.\n";
        } else {
            cout << "Student with ID " << targetId << " not found.\n";
        }
    }
};

int main() {
    StudentManager manager("students.dat");
    int choice;
    
    while (true) {
        cout << "\n--- Student Management System (OOP) ---\n";
        cout << "1. Add Student\n";
        cout << "2. Display All Students\n";
        cout << "3. Update Student\n";
        cout << "4. Delete Student\n";
        cout << "5. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Please enter a number between 1 and 5.\n";
            continue;
        }

        if (choice == 1) manager.addStudent();
        else if (choice == 2) manager.displayAllStudents();
        else if (choice == 3) manager.updateStudent();
        else if (choice == 4) manager.deleteStudent();
        else if (choice == 5) {
            cout << "Exiting program.\n";
            break;
        }
        else {
            cout << "Invalid selection. Try again.\n";
        }
    }
    
    return 0;
}
