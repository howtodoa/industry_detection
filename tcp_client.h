#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <QObject>
#include <QMutex>
#include <QString>
#include <string>
#include <QList>
#include "MZ_ClientControl.h"

// ServerComm 结构体移到类外部
struct ServerComm
{
    CommPorts port;                          // 端口信息
    Mz_ClientControl::ClientOperation* clientOp; // ClientOperation对象
    bool isInitialized;                      // 标记是否已初始化
    bool isRunning;                          // 标记是否正在运行
};

class TCP_Client : public QObject
{
    Q_OBJECT

public:
    explicit TCP_Client(QObject* parent = nullptr);
    ~TCP_Client();

    bool initialize(const std::string& portName); // 初始化某个端口
    bool startWork(const std::string& portName);  // 启动某个端口的工作
    void stopWork(const std::string& portName = ""); // 停止某个端口的工作
    bool doAction(const std::string& portName, const std::string& funcName,
                  const HValues& inputValues, const HImages& inputImages,
                  HValues& outputValues, HImages& outputImages,
                  int& retCode, std::string& retMsg); // 执行远程操作

private:
    ServerComm* findCommByName(const std::string& portName);  // 查找端口
    QList<ServerComm> m_commInstances;  // 存储多个端口通信实例
    QMutex m_mutex;                     // 线程同步锁
};

#endif // TCP_CLIENT_H
