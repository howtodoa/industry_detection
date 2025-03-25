#ifndef ROLE_H
#define ROLE_H

#include <QObject>

class Role : public QObject
{
    Q_OBJECT
public:
    explicit Role(QObject *parent = 0);
    void ChangePassword(const QString &str);
private:
    QString password="12345678";
signals:

public slots:

};

class Operater : public Role
{

};

class Machine : public Role
{

};

class Manufacturer : public Role
{

};













#endif // ROLE_H
