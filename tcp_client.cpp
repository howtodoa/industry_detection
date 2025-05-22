#include "TCP_Client.h"
#include <iostream>

#include<QDebug>

TCP_Client::TCP_Client()
{

}

// TCP_Client::TCP_Client(QObject* parent)
//     : QObject(parent)
// {
// }

TCP_Client::~TCP_Client()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_clientOp) {
        if (m_isRunning) m_isRunning = false;
        m_clientOp->StopWork();
        m_clientOp->FreeSDK();
        delete m_clientOp;
       m_clientOp = nullptr;
        std::cout << "this is the release in _tcpclient" << std::endl;
    }
    std::cout << "this is the release in _tcpclient" << std::endl;
}

bool TCP_Client::initialize(const CommPorts& port)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_isInitialized) {
        std::cout << "Already initialized." << std::endl;
        return false;
    }
    m_port = port;
    m_clientOp = new Mz_ClientControl::ClientOperation();
    m_clientOp->InitSDK(m_port);
    m_isInitialized = true;
         std::cout << "initialize successful" << std::endl;
    return true;
}

bool TCP_Client::startWork()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_isInitialized) {
        std::cout << "Not initialized." << std::endl;
        return false;
    }

    if (m_isRunning) {
        std::cout << "Already running." << std::endl;
        return false;
    }

    m_clientOp->StartWork();
    m_isRunning = true;
    return true;
}

void TCP_Client::stopWork()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (m_isRunning) {
        m_clientOp->StopWork();
        //m_clientOp->FreeSDK();这里只能程序退出的时候调用，否则直接崩溃
        m_isRunning = false;
    }
}

bool TCP_Client::doAction(const std::string& funcName,
                          const HValues& inputValues,
                          const HImages& inputImages,
                          HValues& outputValues,
                          HImages& outputImages,
                          int& retCode, std::string& retMsg)
{
     qDebug()<<"enter TCP_Client::doAction";
    std::lock_guard<std::mutex> lock(m_mutex);

    if (!m_isInitialized) {
        retCode = -1;
        retMsg = "Not initialized.";
        return false;
    }
    if (!m_isRunning) {
        retCode = -2;
        retMsg = "Not running.";
        return false;
    }
    if (!m_clientOp) {
        retCode = -3;
        retMsg = "ClientOperation not available.";
        return false;
    }
qDebug()<<"enter TCP_Client::DoActionFun";
    m_clientOp->DoActionFun(m_port, funcName, inputValues, inputImages, &outputValues, &outputImages, &retCode, &retMsg);
    return (retCode == 0);
}

bool TCP_Client::RegsiterFunitFun(Callbackfunc func)
{
    if (!m_isInitialized) {
        return false;
    }
    if (!m_clientOp) {
        return false;
    }
    m_clientOp->RegsiterFunitFun(func);
    return true;
}

bool TCP_Client::GetFunList_Remote(CommPorts portname,vector<Callbackfunc>& funList)
{
    if (!m_isInitialized) {
        return false;
    }
    if (!m_isRunning) {
        return false;
    }
    if (!m_clientOp) {
        return false;
    }
    m_clientOp->GetFunList_Remote(portname,funList);
    return true;
}

bool TCP_Client::GetRemoteCommports(vector<CommPorts>& commports)
{
    if (!m_isInitialized) {
        return false;
    }
    if (!m_isRunning) {
        return false;
    }
    if (!m_clientOp) {
        return false;
    }
    m_clientOp->GetRemoteCommports(commports);
    return true;
}
