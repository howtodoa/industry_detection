#include "role.h"

// 初始化静态成员变量
QString Role::CurrentRole = "Operator";  // 默认角色
#ifdef  USE_MAIN_WINDOW_BRADER
QString Role::password = "mz2025";     // 默认密码
#else 
QString Role::password = "12345678";     // 默认密码
#endif


QTimer* Role::logoutTimer = nullptr;
QDateTime Role::logoutTime= QDateTime::currentDateTime().addSecs(10 * 60);

Role::Role(const QString &role) : QObject(nullptr)
{
    CurrentRole = role;  // 初始化时设置静态角色
}

Role::~Role()
{

}



void Role::ChangePassword(const QString &str)
{
    password = str;  // 修改静态密码

}

void Role::StartAutoLogout(qint64 milliseconds, QObject* receiver, const std::function<void()>& onLogout)
{
    if (!logoutTimer)
    {
        logoutTimer = new QTimer();
        logoutTimer->setSingleShot(true);
    }

    // 断开旧连接（避免重复）
    QObject::disconnect(logoutTimer, nullptr, nullptr, nullptr);

    // 当计时到达时执行自定义函数
    QObject::connect(logoutTimer, &QTimer::timeout, receiver, [onLogout]() {
        qDebug() << "自动注销: 当前角色" << CurrentRole << "已被注销";
        CurrentRole = "操作员";
        if (onLogout) onLogout();  
        });

    logoutTimer->setInterval(milliseconds);
    logoutTimer->start();

    logoutTime = QDateTime::currentDateTime().addMSecs(milliseconds);
    qDebug() << "自动注销计时开始，将在" << logoutTime.toString("yyyy-MM-dd hh:mm:ss") << "执行";
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
