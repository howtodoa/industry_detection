#include "role.h"

// 初始化静态成员变量
QString Role::CurrentRole = "Operator";  // 默认角色
QString Role::password = "12345678";     // 默认密码

Role::Role(const QString &role) : QObject(nullptr)
{
    CurrentRole = role;  // 初始化时设置静态角色
}

Role::~Role()
{
    // 无需清理静态变量，生命周期由程序控制
}

void Role::ChangePassword(const QString &str)
{
    password = str;  // 修改静态密码

}

void Role::ChangeRole(const QString &newRole)
{
    CurrentRole = newRole;  // 修改静态角色

}

QString Role::GetCurrentRole()
{
    return CurrentRole;  // 返回静态角色
}

bool Role::IsOperator(const QString &role)
{
    return role.compare("Operator", Qt::CaseInsensitive) == 0;
}

bool Role::IsManufacturer(const QString &role)
{
    return role.compare("Manufacturer", Qt::CaseInsensitive) == 0;
}

bool Role::IsMaintenance(const QString &role)
{
    return role.compare("Maintenance", Qt::CaseInsensitive) == 0;
}
