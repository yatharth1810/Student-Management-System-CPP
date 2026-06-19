#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <cstring>

using namespace std;

// Class representing a single Bank Account (Encapsulation)
class Account {
private:
    int accountNumber;
    char accountHolder[50];
    double balance;
    char accountType[20];

public:
    // Default constructor
    Account() {
        accountNumber = 0;
        balance = 0.0;
        strcpy(accountHolder, "");
        strcpy(accountType, "Saving");
    }

    // Parameterized constructor
    Account(int accNum, const char* name, double initialBalance, const char* type) {
        accountNumber = accNum;
        balance = initialBalance;
        strncpy(accountHolder, name, 49);
        accountHolder[49] = '\0';
        strncpy(accountType, type, 19);
        accountType[19] = '\0';
    }

    // Getters
    int getAccountNumber() const { return accountNumber; }
    const char* getAccountHolder() const { return accountHolder; }
    double getBalance() const { return balance; }
    const char* getAccountType() const { return accountType; }

    // Business Methods for Transactions
    void deposit(double amount) {
        balance += amount;
    }

    bool withdraw(double amount) {
        if (amount > balance) {
            return false; // Insufficient funds
        }
        balance -= amount;
        return true;
    }
};

// Class handling core banking system operations
class BankManager {
private:
    string fileName;

public:
    BankManager(string dbName) {
        fileName = dbName;
    }

    // Helper method to verify if an account already exists
    bool accountExists(int accNum) {
        Account acc;
        ifstream inFile(fileName, ios::binary);
        if (!inFile) return false;

        while (inFile.read(reinterpret_cast<char*>(&acc), sizeof(Account))) {
            if (acc.getAccountNumber() == accNum) {
                inFile.close();
                return true;
            }
        }
        inFile.close();
        return false;
    }

    // 1. OPEN ACCOUNT
    void openAccount() {
        int accNum;
        char name[50], type[20];
        double initialDeposit;

        ofstream outFile(fileName, ios::binary | ios::app);
        if (!outFile) {
            cout << "Error: System storage failed to load.\n";
            return;
        }

        cout << "\n--- Open New Account ---\n";
        cout << "Enter Desired Account Number: ";
        cin >> accNum;

        if (accountExists(accNum)) {
            cout << "Error: An account with this number already exists!\n";
            outFile.close();
            return;
        }

        cin.ignore();
        cout << "Enter Account Holder Name: ";
        cin.getline(name, 50);
        
        cout << "Enter Account Type (Saving / Current): ";
        cin.getline(type, 20);
        
        cout << "Enter Initial Deposit Amount: ";
        cin >> initialDeposit;

        if (initialDeposit < 0) {
            cout << "Error: Initial deposit cannot be negative.\n";
            outFile.close();
            return;
        }

        Account newAcc(accNum, name, initialDeposit, type);
        outFile.write(reinterpret_cast<char*>(&newAcc), sizeof(Account));
        outFile.close();

        cout << "Account created successfully.\n";
    }

    // 2. DEPOSIT FUNDS
    void handleDeposit() {
        Account acc;
        fstream file(fileName, ios::binary | ios::in | ios::out);
        int accNum;
        double amount;
        bool found = false;

        if (!file) {
            cout << "\nError opening system files.\n";
            return;
        }

        cout << "\nEnter Account Number for Deposit: ";
        cin >> accNum;

        while (file.read(reinterpret_cast<char*>(&acc), sizeof(Account))) {
            if (acc.getAccountNumber() == accNum) {
                found = true;
                cout << "Account Found -> Holder: " << acc.getAccountHolder() << " | Current Balance: $" << acc.getBalance() << "\n";
                cout << "Enter Amount to Deposit: ";
                cin >> amount;

                if (amount <= 0) {
                    cout << "Invalid deposit amount.\n";
                    break;
                }

                acc.deposit(amount);

                // Rewriting the updated object back to file
                int offset = -1 * static_cast<int>(sizeof(Account));
                file.seekp(offset, ios::cur);
                file.write(reinterpret_cast<char*>(&acc), sizeof(Account));
                
                cout << "Deposit successful! New Balance: $" << acc.getBalance() << "\n";
                break;
            }
        }

        if (!found) cout << "Account number not found.\n";
        file.close();
    }

    // 3. WITHDRAW FUNDS
    void handleWithdrawal() {
        Account acc;
        fstream file(fileName, ios::binary | ios::in | ios::out);
        int accNum;
        double amount;
        bool found = false;

        if (!file) {
            cout << "\nError opening system files.\n";
            return;
        }

        cout << "\nEnter Account Number for Withdrawal: ";
        cin >> accNum;

        while (file.read(reinterpret_cast<char*>(&acc), sizeof(Account))) {
            if (acc.getAccountNumber() == accNum) {
                found = true;
                cout << "Account Found -> Holder: " << acc.getAccountHolder() << " | Current Balance: $" << acc.getBalance() << "\n";
                cout << "Enter Amount to Withdraw: ";
                cin >> amount;

                if (amount <= 0) {
                    cout << "Invalid withdrawal amount.\n";
                    break;
                }

                if (acc.withdraw(amount)) {
                    int offset = -1 * static_cast<int>(sizeof(Account));
                    file.seekp(offset, ios::cur);
                    file.write(reinterpret_cast<char*>(&acc), sizeof(Account));
                    cout << "Withdrawal successful! Remaining Balance: $" << acc.getBalance() << "\n";
                } else {
                    cout << "Transaction Failed: Insufficient funds available.\n";
                }
                break;
            }
        }

        if (!found) cout << "Account number not found.\n";
        file.close();
    }

    // 4. BALANCE INQUIRY
    void checkBalance() {
        Account acc;
        ifstream inFile(fileName, ios::binary);
        int accNum;
        bool found = false;

        if (!inFile) {
            cout << "\nNo records found in database.\n";
            return;
        }

        cout << "\nEnter Account Number: ";
        cin >> accNum;

        while (inFile.read(reinterpret_cast<char*>(&acc), sizeof(Account))) {
            if (acc.getAccountNumber() == accNum) {
                found = true;
                cout << "\n================ ACCOUNT DETAILS ================\n";
                cout << "Account Number : " << acc.getAccountNumber() << "\n";
                cout << "Account Holder : " << acc.getAccountHolder() << "\n";
                cout << "Account Type   : " << acc.getAccountType() << "\n";
                cout << "Current Balance: $" << fixed << setprecision(2) << acc.getBalance() << "\n";
                cout << "=================================================\n";
                break;
            }
        }

        if (!found) cout << "Account number not found.\n";
        inFile.close();
    }
};

int main() {
    BankManager bank("bank_data.dat");
    int choice;

    while (true) {
        cout << "\n--- Core Banking Application ---\n";
        cout << "1. Open New Account\n";
        cout << "2. Deposit Money\n";
        cout << "3. Withdraw Money\n";
        cout << "4. Balance Inquiry\n";
        cout << "5. Exit System\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid format. Try again.\n";
            continue;
        }

        if (choice == 1) bank.openAccount();
        else if (choice == 2) bank.handleDeposit();
        else if (choice == 3) bank.handleWithdrawal();
        else if (choice == 4) bank.checkBalance();
        else if (choice == 5) {
            cout << "Terminating banking session.\n";
            break;
        }
        else {
            cout << "Invalid option chosen.\n";
        }
    }

    return 0;
}
