#include "TCP_Client.h"
#include <QDebug>
#include <QMutexLocker>

TCP_Client::TCP_Client(QObject* parent) : QObject(parent) {}

TCP_Client::~TCP_Client()
{
    // 释放所有的客户端操作对象
    for (auto& comm : m_commInstances) {
        if (comm.clientOp) {
            comm.clientOp->FreeSDK();  // 如果需要调用 FreeSDK 进行资源清理，可以放在这里
            delete comm.clientOp;  // 删除 ClientOperation 对象
            comm.clientOp = nullptr;  // 设置为 nullptr 防止悬挂指针
        }
    }
}
bool TCP_Client::initialize(const std::string& portName)
{
    QMutexLocker locker(&m_mutex);

    ServerComm* comm = findCommByName(portName);
    if (!comm) {
        qDebug() << "Port not found:" << QString::fromStdString(portName);
        return false;
    }

    if (comm->isInitialized) {
        qDebug() << "Port already initialized:" << QString::fromStdString(portName);
        return false;
    }

    comm->clientOp = new Mz_ClientControl::ClientOperation();
    comm->clientOp->InitSDK(comm->port);
    comm->isInitialized = true;
    return true;
}

bool TCP_Client::startWork(const std::string& portName)
{
    QMutexLocker locker(&m_mutex);

    ServerComm* comm = findCommByName(portName);
    if (!comm) {
        qDebug() << "Port not found:" << QString::fromStdString(portName);
        return false;
    }

    if (!comm->isInitialized) {
        qDebug() << "Port not initialized:" << QString::fromStdString(portName);
        return false;
    }

    if (comm->isRunning) {
        qDebug() << "Port already running:" << QString::fromStdString(portName);
        return false;
    }

    comm->clientOp->StartWork();
    comm->isRunning = true;
    return true;
}

void TCP_Client::stopWork(const std::string& portName)
{
    QMutexLocker locker(&m_mutex);

    if (portName.empty()) {
        for (auto& comm : m_commInstances) {
            if (comm.isRunning) {
                comm.clientOp->StopWork();
                comm.isRunning = false;
            }
        }
    } else {
        ServerComm* comm = findCommByName(portName);
        if (comm && comm->isRunning) {
            comm->clientOp->StopWork();
            comm->isRunning = false;
        }
    }
}

bool TCP_Client::doAction(const std::string& portName, const std::string& funcName,
                          const HValues& inputValues, const HImages& inputImages,
                          HValues& outputValues, HImages& outputImages,
                          int& retCode, std::string& retMsg)
{
    QMutexLocker locker(&m_mutex);

    ServerComm* comm = findCommByName(portName);
    if (!comm) {
        retCode = -1;
        retMsg = "Port not found.";
        return false;
    }

    if (!comm->isInitialized) {
        retCode = -2;
        retMsg = "Port not initialized.";
        return false;
    }

    if (!comm->isRunning) {
        retCode = -3;
        retMsg = "Port not running.";
        return false;
    }

    if (comm->clientOp) {
        comm->clientOp->DoActionFun(comm->port, funcName, inputValues, inputImages,
                                    &outputValues, &outputImages, &retCode, &retMsg);
        return (retCode == 0);
    }

    retCode = -4;
    retMsg = "Client operation not available.";
    return false;
}

ServerComm* TCP_Client::findCommByName(const std::string& portName)
{
    for (auto& comm : m_commInstances) {
        if (comm.port.PortName == portName) {
            return &comm;
        }
    }
    return nullptr;
}
