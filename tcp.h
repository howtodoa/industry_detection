#ifndef TCP_H
#define TCP_H

#include "MZ_ClientControl.h"
#include <QObject>
#include <QString>
#include <vector>
#include <string>

// TCP 类继承 QObject，这样可以使用 Qt 的信号和槽机制（如果需要的话）
class TCP : public QObject
{
    Q_OBJECT

public:
    // 构造函数与析构函数
    TCP();
    ~TCP();

    // SDK 初始化与释放
    bool initialize(const std::vector<CommPorts>& ports);
    void cleanup();

    // 启动工作与停止工作
    bool startWork();
    void stopWork();

    // 执行远程操作
    bool doAction(const std::string& portName, const std::string& funcName,
                  const HValues& inputValues, const HImages& inputImages,
                  HValues& outputValues, HImages& outputImages,
                  int& retCode, std::string& retMsg);

    // 注册本地功能
    bool registerLocalFunction(const Callbackfunc& func);

    // 获取远程功能列表
    bool getRemoteFunctionList(const std::string& portName, std::vector<Callbackfunc>& funList);

    // 获取远程通信端口列表
    bool getRemoteCommPorts(std::vector<CommPorts>& commports);

private:

    Mz_ClientControl::ClientOperation* m_client;

    std::vector<CommPorts> m_ports;
};

#endif // TCP_H
