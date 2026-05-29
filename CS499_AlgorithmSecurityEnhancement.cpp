#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/*
    CS 499 Milestone Three Algorithms and Data Structures Enhancement
    Artifact: CS 405 Secure Coding Portfolio - Module One Overflow Project
    Student: Erek Mackroy

    Purpose:
    This enhanced code demonstrates algorithmic security logic through
    input validation, failed login counters, risk scoring, decision-based checks,
    temporary account lockout logic, and a security event log.

    This enhancement builds on the original CS 405 buffer overflow artifact by
    showing how structured logic and data tracking can reduce unsafe input and
    suspicious behavior before it becomes a larger security problem.
*/

struct SecurityEvent {
    string eventType;
    string message;
    int riskValue;
};

struct UserAccount {
    string username;
    string passwordHash;
    int failedLoginCount;
    int riskScore;
    bool locked;
};

bool containsUnsafeCharacters(const string& input) {
    const string unsafeCharacters = "'\";--<>[]{}()";

    for (char c : input) {
        if (unsafeCharacters.find(c) != string::npos) {
            return true;
        }
    }

    return false;
}

bool isWithinLengthLimit(const string& input, int maxLength) {
    return !input.empty() && input.length() <= maxLength;
}

bool isValidInput(const string& input) {
    if (!isWithinLengthLimit(input, 30)) {
        return false;
    }

    if (containsUnsafeCharacters(input)) {
        return false;
    }

    return true;
}

void addSecurityEvent(vector<SecurityEvent>& eventLog, const string& eventType, const string& message, int riskValue) {
    eventLog.push_back({ eventType, message, riskValue });
}

string hashPasswordDemo(const string& password) {
    /*
        Demonstration only:
        In a real system, passwords should be hashed with bcrypt, Argon2,
        or PBKDF2 using a salt. This simplified function is only used to
        demonstrate the algorithmic flow.
    */
    return "HASHED_" + password;
}

UserAccount* findAccount(vector<UserAccount>& accounts, const string& username) {
    for (UserAccount& account : accounts) {
        if (account.username == username) {
            return &account;
        }
    }

    return nullptr;
}

void increaseRisk(UserAccount& account, int riskAmount) {
    account.riskScore += riskAmount;

    if (account.failedLoginCount >= 3 || account.riskScore >= 10) {
        account.locked = true;
    }
}

bool processLoginAttempt(vector<UserAccount>& accounts, vector<SecurityEvent>& eventLog,
                         const string& username, const string& password) {
    if (!isValidInput(username) || !isValidInput(password)) {
        addSecurityEvent(eventLog, "INPUT_REJECTED",
            "Login attempt rejected because input failed validation.", 5);

        cout << "Login failed. Please check your credentials." << endl;
        return false;
    }

    UserAccount* account = findAccount(accounts, username);

    if (account == nullptr) {
        addSecurityEvent(eventLog, "UNKNOWN_USER",
            "Login attempt used an unknown username.", 3);

        cout << "Login failed. Please check your credentials." << endl;
        return false;
    }

    if (account->locked) {
        addSecurityEvent(eventLog, "ACCOUNT_LOCKED",
            "Login attempt blocked because account is locked.", 10);

        cout << "Account is temporarily locked. Please contact support." << endl;
        return false;
    }

    string enteredPasswordHash = hashPasswordDemo(password);

    if (enteredPasswordHash != account->passwordHash) {
        account->failedLoginCount++;
        increaseRisk(*account, 3);

        addSecurityEvent(eventLog, "LOGIN_FAILED",
            "Failed login attempt recorded and risk score updated.", 3);

        cout << "Login failed. Please check your credentials." << endl;
        return false;
    }

    account->failedLoginCount = 0;
    account->riskScore = 0;

    addSecurityEvent(eventLog, "LOGIN_SUCCESS",
        "Successful login. Failed login counter and risk score reset.", 0);

    cout << "Login successful." << endl;
    return true;
}

void printAccountStatus(const vector<UserAccount>& accounts) {
    cout << "\nAccount Status Report" << endl;

    for (const UserAccount& account : accounts) {
        cout << "Username: " << account.username << endl;
        cout << "Failed Login Count: " << account.failedLoginCount << endl;
        cout << "Risk Score: " << account.riskScore << endl;
        cout << "Locked: " << (account.locked ? "Yes" : "No") << endl;
        cout << "------------------------" << endl;
    }
}

void printSecurityEventLog(const vector<SecurityEvent>& eventLog) {
    cout << "\nSecurity Event Log" << endl;

    for (const SecurityEvent& event : eventLog) {
        cout << event.eventType << " | Risk +" << event.riskValue
             << " | " << event.message << endl;
    }
}

int main() {
    vector<UserAccount> accounts = {
        { "admin", hashPasswordDemo("AdminPass123"), 0, 0, false },
        { "employee", hashPasswordDemo("EmployeePass123"), 0, 0, false },
        { "user", hashPasswordDemo("UserPass123"), 0, 0, false }
    };

    vector<SecurityEvent> eventLog;

    string username;
    string password;

    cout << "CS 499 Algorithm Security Enhancement" << endl;
    cout << "This program demonstrates failed login counters, risk scoring," << endl;
    cout << "input validation, lockout logic, and security event logging." << endl;

    for (int attempt = 1; attempt <= 3; attempt++) {
        cout << "\nLogin Attempt " << attempt << endl;
        cout << "Username: ";
        cin >> username;

        cout << "Password: ";
        cin >> password;

        processLoginAttempt(accounts, eventLog, username, password);
    }

    printAccountStatus(accounts);
    printSecurityEventLog(eventLog);

    return 0;
}