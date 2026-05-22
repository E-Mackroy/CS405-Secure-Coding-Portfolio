#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/*
    CS 499 Milestone Two Software Design/Engineering Enhancement
    Artifact: CS 405 Secure Coding Portfolio
    Student: Erek Mackroy

    Purpose:
    This enhanced code demonstrates secure software design concepts,
    including input validation, secure login flow, role-based access control,
    safer error handling, and security logging.
*/

enum class UserRole {
    StandardUser,
    Employee,
    Administrator
};

struct User {
    string username;
    string passwordHash;
    UserRole role;
};

struct SecurityEvent {
    string eventType;
    string message;
};

bool containsUnsafeCharacters(const string& input) {
    const string unsafeCharacters = "'\";--<>";

    for (char c : input) {
        if (unsafeCharacters.find(c) != string::npos) {
            return true;
        }
    }

    return false;
}

bool isValidInput(const string& input) {
    if (input.empty()) {
        return false;
    }

    if (input.length() > 30) {
        return false;
    }

    if (containsUnsafeCharacters(input)) {
        return false;
    }

    return true;
}

void logSecurityEvent(vector<SecurityEvent>& log, const string& eventType, const string& message) {
    log.push_back({ eventType, message });
}

string hashPasswordDemo(const string& password) {
    /*
        Demonstration only:
        In a real system, passwords should be hashed with a strong password
        hashing algorithm such as bcrypt, Argon2, or PBKDF2 with a salt.
    */
    return "HASHED_" + password;
}

User* findUser(vector<User>& users, const string& username) {
    for (User& user : users) {
        if (user.username == username) {
            return &user;
        }
    }

    return nullptr;
}

bool hasPermission(UserRole role, const string& requestedAction) {
    if (role == UserRole::Administrator) {
        return true;
    }

    if (role == UserRole::Employee && requestedAction == "VIEW_REPORTS") {
        return true;
    }

    if (role == UserRole::StandardUser && requestedAction == "VIEW_PROFILE") {
        return true;
    }

    return false;
}

bool secureLogin(vector<User>& users, vector<SecurityEvent>& log, const string& username, const string& password) {
    if (!isValidInput(username) || !isValidInput(password)) {
        logSecurityEvent(log, "LOGIN_REJECTED", "Invalid or unsafe login input was rejected.");
        cout << "Login failed. Please check your credentials." << endl;
        return false;
    }

    User* user = findUser(users, username);

    if (user == nullptr) {
        logSecurityEvent(log, "LOGIN_FAILED", "Login failed for unknown user.");
        cout << "Login failed. Please check your credentials." << endl;
        return false;
    }

    string enteredPasswordHash = hashPasswordDemo(password);

    if (enteredPasswordHash != user->passwordHash) {
        logSecurityEvent(log, "LOGIN_FAILED", "Login failed for known user.");
        cout << "Login failed. Please check your credentials." << endl;
        return false;
    }

    logSecurityEvent(log, "LOGIN_SUCCESS", "User logged in successfully.");
    cout << "Login successful." << endl;
    return true;
}

void attemptProtectedAction(UserRole role, const string& requestedAction, vector<SecurityEvent>& log) {
    if (hasPermission(role, requestedAction)) {
        logSecurityEvent(log, "ACCESS_GRANTED", "User was granted access to requested action.");
        cout << "Access granted for action: " << requestedAction << endl;
    }
    else {
        logSecurityEvent(log, "ACCESS_DENIED", "User attempted unauthorized action.");
        cout << "Access denied. You do not have permission for this action." << endl;
    }
}

void printDeploymentChecklist() {
    cout << "\nDeployment Security Checklist" << endl;
    cout << "1. Login credentials are validated before processing." << endl;
    cout << "2. Passwords are never stored or displayed in plain text." << endl;
    cout << "3. Failed login attempts are logged." << endl;
    cout << "4. General error messages are shown to users." << endl;
    cout << "5. User roles are checked before protected actions are allowed." << endl;
    cout << "6. Standard users cannot access administrator functions." << endl;
    cout << "7. User input is checked for length, type, format, and unsafe characters." << endl;
    cout << "8. Database queries should use safe query handling." << endl;
    cout << "9. Security events are logged for later review." << endl;
    cout << "10. Sensitive data is not written to logs or error messages." << endl;
}

void printSecurityLog(const vector<SecurityEvent>& log) {
    cout << "\nSecurity Event Log" << endl;

    for (const SecurityEvent& event : log) {
        cout << event.eventType << ": " << event.message << endl;
    }
}

int main() {
    vector<SecurityEvent> securityLog;

    vector<User> users = {
        { "admin", hashPasswordDemo("AdminPass123"), UserRole::Administrator },
        { "employee", hashPasswordDemo("EmployeePass123"), UserRole::Employee },
        { "user", hashPasswordDemo("UserPass123"), UserRole::StandardUser }
    };

    string username;
    string password;

    cout << "Secure Login Demonstration" << endl;
    cout << "Username: ";
    cin >> username;

    cout << "Password: ";
    cin >> password;

    bool loginSuccessful = secureLogin(users, securityLog, username, password);

    if (loginSuccessful) {
        User* currentUser = findUser(users, username);

        if (currentUser != nullptr) {
            attemptProtectedAction(currentUser->role, "VIEW_PROFILE", securityLog);
            attemptProtectedAction(currentUser->role, "ADMIN_SETTINGS", securityLog);
        }
    }

    printDeploymentChecklist();
    printSecurityLog(securityLog);

    return 0;
}