#include <iostream>
#include <fstream>
#include <map>
#include <string>

using namespace std;

#define MIN_BALANCE 500

class InsufficientFunds {};

class Account {
private:
    long accountNumber;
    string firstName;
    string lastName;
    float balance;
    static long NextAccountNumber;
public:
    Account() {}
    Account(string fname, string lname, float balance);
    long getAccountNumber() { return accountNumber; }
    string getFirstName() { return firstName; }
    string getLastName() { return lastName; }
    float getBalance() { return balance; }

    void Deposit(float amount);
    void Withdraw(float amount);
    static void setLastAccountNumber(long accountNumber);
    static long getLastAccountNumber();

    // Friend functions for file and console IO
    friend ofstream & operator<<(ofstream &ofs, Account &acc);
    friend ifstream & operator>>(ifstream &ifs, Account &acc);
    friend ostream & operator<<(ostream &os, Account &acc);
};

long Account::NextAccountNumber = 0;

Account::Account(string fname, string lname, float balance) {
    NextAccountNumber++;
    accountNumber = NextAccountNumber;
    firstName = fname;
    lastName = lname;
    this->balance = balance;
}

void Account::Deposit(float amount) {
    balance += amount;
}

void Account::Withdraw(float amount) {
    // Check if the balance after withdrawal doesn't go below minimum
    if (balance - amount < MIN_BALANCE) {
        throw InsufficientFunds();
    }
    balance -= amount;
}

void Account::setLastAccountNumber(long accountNumber) {
    NextAccountNumber = accountNumber;
}

long Account::getLastAccountNumber() {
    return NextAccountNumber;
}

// Writes the account details to file
ofstream & operator<<(ofstream &ofs, Account &acc) {
    ofs << acc.accountNumber << endl;
    ofs << acc.firstName << endl;
    ofs << acc.lastName << endl;
    ofs << acc.balance << endl;
    return ofs;
}

// Reads the account details from file
ifstream & operator>>(ifstream &ifs, Account &acc) {
    ifs >> acc.accountNumber >> acc.firstName >> acc.lastName >> acc.balance;
    return ifs;
}

// Outputs the account details to the console
ostream & operator<<(ostream &os, Account &acc) {
    os << "Account Number: " << acc.getAccountNumber() << endl;
    os << "First Name: " << acc.getFirstName() << endl;
    os << "Last Name: " << acc.getLastName() << endl;
    os << "Balance: $" << acc.getBalance() << endl;
    return os;
}

// The Bank class coordinates actions and maintains a map of Accounts
class Bank {
private:
    map<long, Account> accounts;
public:
    Bank();
    Account OpenAccount(string fname, string lname, float balance);
    Account BalanceEnquiry(long accountNumber);
    Account Deposit(long accountNumber, float amount);
    Account Withdraw(long accountNumber, float amount);
    void CloseAccount(long accountNumber);
    void ShowAllAccounts();
    ~Bank();
};

Bank::Bank() {
    Account account;
    ifstream infile("Bank.data");
    
    // File might not exist the first time the program runs
    if (!infile) {
        return; 
    }
    
    // Load accounts into the map
    while (infile >> account) {
        accounts.insert(pair<long, Account>(account.getAccountNumber(), account));
    }
    Account::setLastAccountNumber(account.getAccountNumber());
}

Account Bank::OpenAccount(string fname, string lname, float balance) {
    ofstream outfile("Bank.data", ios::trunc); // Overwrite file to update
    Account account(fname, lname, balance);
    accounts.insert(pair<long, Account>(account.getAccountNumber(), account));

    map<long, Account>::iterator itr;
    for (itr = accounts.begin(); itr != accounts.end(); itr++) {
        outfile << itr->second;
    }
    return account;
}

Account Bank::BalanceEnquiry(long accountNumber) {
    map<long, Account>::iterator itr = accounts.find(accountNumber);
    if(itr != accounts.end()) return itr->second;
    throw "Account not found";
}

Account Bank::Deposit(long accountNumber, float amount) {
    map<long, Account>::iterator itr = accounts.find(accountNumber);
    if(itr != accounts.end()) {
        itr->second.Deposit(amount);
        return itr->second;
    }
    throw "Account not found";
}

Account Bank::Withdraw(long accountNumber, float amount) {
    map<long, Account>::iterator itr = accounts.find(accountNumber);
    if(itr != accounts.end()) {
        itr->second.Withdraw(amount);
        return itr->second;
    }
    throw "Account not found";
}

void Bank::CloseAccount(long accountNumber) {
    map<long, Account>::iterator itr = accounts.find(accountNumber);
    if(itr != accounts.end()) {
        cout << "Account Deleted: " << endl << itr->second << endl;
        accounts.erase(accountNumber);
    } else {
        throw "Account not found";
    }
}

void Bank::ShowAllAccounts() {
    map<long, Account>::iterator itr;
    if (accounts.empty()) {
        cout << "No accounts to display." << endl;
        return;
    }
    for (itr = accounts.begin(); itr != accounts.end(); itr++) {
        cout << "--- Account " << itr->first << " ---" << endl;
        cout << itr->second << endl;
    }
}

Bank::~Bank() {
    // When the application shuts down, persist state
    ofstream outfile("Bank.data", ios::trunc);
    map<long, Account>::iterator itr;
    for (itr = accounts.begin(); itr != accounts.end(); itr++) {
        outfile << itr->second;
    }
}

int main() {
    Bank b;
    Account acc;
    int choice;
    string fname, lname;
    long accountNumber;
    float balance;
    float amount;

    cout << "=======================================" << endl;
    cout << "     BANK MANAGEMENT SYSTEM IN C++     " << endl;
    cout << "=======================================" << endl;

    do {
        cout << "\n--- Main Menu ---";
        cout << "\n1. Open an Account";
        cout << "\n2. Balance Enquiry";
        cout << "\n3. Deposit";
        cout << "\n4. Withdrawal";
        cout << "\n5. Close an Account";
        cout << "\n6. Show All Accounts";
        cout << "\n7. Quit";
        cout << "\n\nEnter your choice: ";
        cin >> choice;

        try {
            switch (choice) {
                case 1:
                    cout << "Enter First Name: ";
                    cin >> fname;
                    cout << "Enter Last Name: ";
                    cin >> lname;
                    cout << "Enter Initial Balance: ";
                    cin >> balance;
                    // Prevent creating an account below minimum balance if you want
                    if (balance < MIN_BALANCE) {
                        cout << "Failed: Initial balance must be at least $" << MIN_BALANCE << endl;
                        break;
                    }
                    acc = b.OpenAccount(fname, lname, balance);
                    cout << "\n=> Congratulations! Account is Created" << endl;
                    cout << acc;
                    break;
                case 2:
                    cout << "Enter Account Number: ";
                    cin >> accountNumber;
                    acc = b.BalanceEnquiry(accountNumber);
                    cout << "\n=> Your Account Details" << endl;
                    cout << acc;
                    break;
                case 3:
                    cout << "Enter Account Number: ";
                    cin >> accountNumber;
                    cout << "Enter Amount to Deposit: ";
                    cin >> amount;
                    acc = b.Deposit(accountNumber, amount);
                    cout << "\n=> Amount properly deposited" << endl;
                    cout << acc;
                    break;
                case 4:
                    cout << "Enter Account Number: ";
                    cin >> accountNumber;
                    cout << "Enter Amount to Withdraw: ";
                    cin >> amount;
                    acc = b.Withdraw(accountNumber, amount);
                    cout << "\n=> Amount Successfully Withdrawn" << endl;
                    cout << acc;
                    break;
                case 5:
                    cout << "Enter Account Number: ";
                    cin >> accountNumber;
                    b.CloseAccount(accountNumber);
                    cout << "=> Account properly closed." << endl;
                    break;
                case 6:
                    cout << "\n=> List of All Accounts" << endl;
                    b.ShowAllAccounts();
                    break;
                case 7:
                    cout << "\nThank you for using the Bank Management System. Goodbye!" << endl;
                    break;
                default:
                    cout << "=> Invalid choice! Please enter a valid menu option." << endl;
            }
        } catch (InsufficientFunds) {
            cout << "=> Error: Insufficient Funds! Minimum balance requirement is $" << MIN_BALANCE << endl;
        } catch (const char* msg) {
            cout << "=> Error: " << msg << endl;
        }
    } while (choice != 7);
    
    return 0;
}
