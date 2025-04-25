#include "TCP_Client.h"
#include <QDebug>
#include <QString>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <string>  // 引入 std::string

TCP_Client::TCP_Client(QObject* parent) : QObject(parent) {}

TCP_Client::~TCP_Client()
{
    // 停止所有端口的工作
    stopWork();
}

bool TCP_Client::initialize(const QList<CommPorts>& ports)
{
    QMutexLocker locker(&m_mutex); // 锁住互斥量以保证线程安全
    m_ports = ports;
    m_commInstances.clear();

    // 为每个端口创建通信实例
    for (const auto& port : m_ports) {
        ServerComm comm;
        comm.port = port;
        comm.isRunning = false; // 初始状态为停止
        m_commInstances.push_back(comm);
    }

    return true;
}

bool TCP_Client::startWork(const std::string& portName)
{
    QMutexLocker locker(&m_mutex);

    for (auto& comm : m_commInstances) {
        if (comm.port.PortName == portName && !comm.isRunning) {
            comm.isRunning = true;
            // 在这里启动端口的工作（例如，初始化套接字，开始监听等）
            qDebug() << "Started work on port:" << QString::fromStdString(portName);
            return true;
        }
    }

    return false;
}

void TCP_Client::stopWork(const std::string& portName)
{
    QMutexLocker locker(&m_mutex);

    if (portName.empty()) {
        // 如果传入空字符串，停止所有端口的工作
        for (auto& comm : m_commInstances) {
            if (comm.isRunning) {
                comm.isRunning = false;
                qDebug() << "Stopped work on port:" << QString::fromStdString(comm.port.PortName);
            }
        }
    } else {
        // 停止指定端口的工作
        for (auto& comm : m_commInstances) {
            if (comm.port.PortName == portName && comm.isRunning) {
                comm.isRunning = false;
                qDebug() << "Stopped work on port:" << QString::fromStdString(portName);
                return;
            }
        }
    }
}

bool TCP_Client::doAction(const std::string& portName, const std::string& funcName,
                          const QList<int>& inputValues, const QList<int>& inputImages,
                          QList<int>& outputValues, QList<int>& outputImages,
                          int& retCode, std::string& retMsg)
{
    QMutexLocker locker(&m_mutex);

    for (auto& comm : m_commInstances) {
        if (comm.port.PortName == portName && comm.isRunning) {
            // 在这里模拟远程函数的调用
            qDebug() << "Calling function" << QString::fromStdString(funcName) << "on port:" << QString::fromStdString(portName);

            // 执行函数，这里用模拟的逻辑代替实际的网络操作
            // 假设操作成功，返回结果
            retCode = 0;
            retMsg = "Success";

            // 模拟处理图像和参数
            outputValues = inputValues;  // 假设输出等于输入
            outputImages = inputImages;  // 假设输出等于输入

            return true;
        }
    }

    retCode = -1;
    retMsg = "Port not running or not found.";
    return false;
}
