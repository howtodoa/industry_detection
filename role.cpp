#include "role.h"

Role::Role(QObject *parent) :
    QObject(parent)
{

}

void Role::ChangePassword(const QString &str)
{
    this->password=str;
}

