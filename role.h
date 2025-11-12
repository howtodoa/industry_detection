#ifndef ROLE_H
#define ROLE_H

#include <QObject>
#include <QTimer>
#include <QDateTime>

class Role : public QObject
{
    Q_OBJECT
public:
    explicit Role(const QString &currentRole = "操作员"); // 构造函数仍保留，用于信号绑定
    ~Role();

    // 静态方法
    static void ChangePassword(const QString &str);       // 修改密码
    static void ChangeRole(const QString &newRole);       // 修改角色接口
    static bool IsOperator(const QString &role);          // 检查是否为操作员
    static bool IsMaintenance(const QString &role);       // 检查是否为机修
    static bool IsManufacturer(const QString &role);      // 检查是否为厂商
    static QString GetCurrentRole();                      // 获取当前角色
	static void StartAutoLogout(qint64 milliseconds, QObject* receiver, const std::function<void()>& onLogout); // 启动自动注销计时器


    // 静态成员变量
    static QString CurrentRole;  // 当前角色（静态共享）

signals:
    void loginSuccess(const QString &role);  // 信号保留，但需实例触发

private:
    static QString password;  // 密码（静态共享）

private:
    static QTimer* logoutTimer;          // 注销计时器
    static QDateTime logoutTime;         // 预定的注销时间点
};

#endif // ROLE_H
