#pragma once

#include <QString>
#include "person.h"

class User {
private:
    Person person;
    QString username;
    QString role;
    QString cookie;
    int userId;
    bool authorized;

public:
    User();
    User(const Person& person, const QString& username, const QString& role, int id);
    User(const QString& firstName, const QString& lastName, const QString& middleName,
         const QDate& birthDate, const QString& phoneNumber, const QString& email,
         const QString& username, const QString& role, int id);
    ~User();

    Person getPerson() const;
    QString getUsername() const;
    int getUserId() const;
    bool isAuthorized() const;
    QString getRole() const;
    QString getCookie() const;
    
    void setUserId(int id);
    void setPerson(const Person& person);
    void setUsername(const QString& username);
    void setAuthorized(bool authorized);
    void setRole(const QString& role);

    void checkCookieFile();
    bool cookieFileExists();

    QString toString() const;
    void displayInfo() const;
};