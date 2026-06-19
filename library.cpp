#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstring>

using namespace std;

class Book {
private:
    int id;
    char title[50];
    char author[50];
    bool isIssued;

public:
    Book() {
        id = 0;
        strcpy(title, "");
        strcpy(author, "");
        isIssued = false;
    }

    Book(int bId, const char* bTitle, const char* bAuthor) {
        id = bId;
        isIssued = false;
        strncpy(title, bTitle, 49);
        title[49] = '\0';
        strncpy(author, bAuthor, 49);
        author[49] = '\0';
    }

    int getId() const { return id; }
    const char* getTitle() const { return title; }
    const char* getAuthor() const { return author; }
    bool getIssuedStatus() const { return isIssued; }

    void setIssuedStatus(bool status) { isIssued = status; }
};

class LibraryManager {
private:
    string fileName;

public:
    LibraryManager(string dbName) {
        fileName = dbName;
    }

    void addBook() {
        int id;
        char title[50], author[50];

        ofstream outFile(fileName, ios::binary | ios::app);
        if (!outFile) {
            cout << "Storage file error.\n";
            return;
        }

        cout << "\n--- Add New Book ---\n";
        cout << "Enter Book ID: ";
        cin >> id;
        cin.ignore();
        cout << "Enter Title: ";
        cin.getline(title, 50);
        cout << "Enter Author: ";
        cin.getline(author, 50);

        Book b(id, title, author);
        outFile.write(reinterpret_cast<char*>(&b), sizeof(Book));
        outFile.close();
        cout << "Book added to inventory.\n";
    }

    void displayAllBooks() {
        Book b;
        ifstream inFile(fileName, ios::binary);
        if (!inFile) {
            cout << "\nLibrary catalog is empty.\n";
            return;
        }

        cout << "\n==================== LIBRARY CATALOG ====================\n";
        cout << left << setw(10) << "ID" << setw(25) << "Title" << setw(20) << "Author" << setw(15) << "Status" << endl;
        cout << "---------------------------------------------------------\n";
        while (inFile.read(reinterpret_cast<char*>(&b), sizeof(Book))) {
            cout << left << setw(10) << b.getId()
                 << setw(25) << b.getTitle()
                 << setw(20) << b.getAuthor()
                 << setw(15) << (b.getIssuedStatus() ? "Issued" : "Available") << endl;
        }
        cout << "=========================================================\n";
        inFile.close();
    }

    void searchBook() {
        Book b;
        ifstream inFile(fileName, ios::binary);
        if (!inFile) return;

        int choice;
        char query[50];
        bool found = false;

        cout << "\nSearch by: 1. Title | 2. Author: ";
        cin >> choice;
        cin.ignore();
        cout << "Enter keyword: ";
        cin.getline(query, 50);

        while (inFile.read(reinterpret_cast<char*>(&b), sizeof(Book))) {
            if ((choice == 1 && strstr(b.getTitle(), query) != NULL) ||
                (choice == 2 && strstr(b.getAuthor(), query) != NULL)) {
                cout << "\nFound -> ID: " << b.getId() << " | Title: " << b.getTitle() 
                     << " | Author: " << b.getAuthor() << " [" << (b.getIssuedStatus() ? "Issued" : "Available") << "]\n";
                found = true;
            }
        }
        if (!found) cout << "No matching books found.\n";
        inFile.close();
    }

    void processTransaction(bool issueMode) {
        Book b;
        fstream file(fileName, ios::binary | ios::in | ios::out);
        int targetId;
        bool found = false;

        if (!file) return;
        cout << "\nEnter Book ID to " << (issueMode ? "issue" : "return") << ": ";
        cin >> targetId;

        while (file.read(reinterpret_cast<char*>(&b), sizeof(Book))) {
            if (b.getId() == targetId) {
                found = true;
                if (issueMode && b.getIssuedStatus()) {
                    cout << "Book is already checked out.\n";
                } else if (!issueMode && !b.getIssuedStatus()) {
                    cout << "Book is already in the library.\n";
                } else {
                    b.setIssuedStatus(issueMode);
                    int offset = -1 * static_cast<int>(sizeof(Book));
                    file.seekp(offset, ios::cur);
                    file.write(reinterpret_cast<char*>(&b), sizeof(Book));
                    cout << "Transaction processed successfully.\n";
                }
                break;
            }
        }
        if (!found) cout << "Book ID not found.\n";
        file.close();
    }
};

int main() {
    LibraryManager lib("library.dat");
    int choice;

    while (true) {
        cout << "\n--- Library Management System ---\n";
        cout << "1. Add Book\n2. View All Books\n3. Search Book\n4. Issue Book\n5. Return Book\n6. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) lib.addBook();
        else if (choice == choice == 2) lib.displayAllBooks();
        else if (choice == 3) lib.searchBook();
        else if (choice == 4) lib.processTransaction(true);
        else if (choice == 5) lib.processTransaction(false);
        else if (choice == 6) break;
        else cout << "Invalid option.\n";
    }
    return 0;
}
