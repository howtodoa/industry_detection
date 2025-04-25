#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <QObject>
#include <QMutex>
#include <QString>
#include <QList>
#include <string>  // 引入 std::string
#include "MZ_ClientControl.h"

class TCP_Client : public QObject
{
    Q_OBJECT

public:
    explicit TCP_Client(QObject* parent = nullptr);
    ~TCP_Client();

    // 初始化多个端口
    bool initialize(const QList<CommPorts>& ports);

    // 启动某个端口的工作
    bool startWork(const std::string& portName);

    // 停止某个端口的工作
    void stopWork(const std::string& portName = "");

    // 执行远程操作
    bool doAction(const std::string& portName, const std::string& funcName,
                  const QList<int>& inputValues, const QList<int>& inputImages,
                  QList<int>& outputValues, QList<int>& outputImages,
                  int& retCode, std::string& retMsg);

private:
    struct ServerComm
    {
        CommPorts port;
        bool isRunning;
    };

    QList<ServerComm> m_commInstances; // 用于管理多个端口的通信实例
    QList<CommPorts> m_ports;         // 存储端口信息
    QMutex m_mutex;                   // 线程同步锁
};

#endif // TCP_CLIENT_H
