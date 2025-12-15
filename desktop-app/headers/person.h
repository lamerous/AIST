#pragma once

#include <QString>
#include <QDate>
#include <QDebug>

class Person {
protected:
    QString firstName;
    QString lastName;
    QString middleName;
    QDate birthDate;
    QString phoneNumber;
    QString email;

public:
    Person();
    Person(const QString& firstName, const QString& lastName, const QString& middleName,
           const QDate& birthDate, const QString& phoneNumber, const QString& email);
    virtual ~Person();

    QString getFirstName() const;
    QString getLastName() const;
    QString getMiddleName() const;
    QString getFullName() const;
    QDate getBirthDate() const;
    QString getPhoneNumber() const;
    QString getEmail() const;

    void setFirstName(const QString& firstName);
    void setLastName(const QString& lastName);
    void setMiddleName(const QString& middleName);
    void setBirthDate(const QDate& birthDate);
    void setPhoneNumber(const QString& phoneNumber);
    void setEmail(const QString& email);

    virtual QString getRole() const;
    virtual void displayInfo() const;
    virtual QString toString() const;

    int getAge() const;
    bool isValid() const;
};

QDebug operator<<(QDebug debug, const Person& person);