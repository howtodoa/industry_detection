#include "tcp.h"
#include <QDebug>

TCP::TCP() : QObject(nullptr), m_client(nullptr) {}

TCP::~TCP() {
    cleanup();
}

bool TCP::initialize(const std::vector<CommPorts>& ports) {
    m_client = new Mz_ClientControl::ClientOperation();
    if (!m_client) {
        qDebug() << "Failed to create ClientOperation instance" ;
        return false;
    }

    m_client->InitSDK(ports);
    m_client->StartWork();
    return true;
}

bool TCP::doAction(const std::string& portName, const std::string& funcName,
                   const HValues& inputValues, const HImages& inputImages,
                   HValues& outputValues, HImages& outputImages,
                   int& retCode, std::string& retMsg) {
    if (!m_client) {
       qDebug() << "Client not initialized" ;
        retCode = -1;
        retMsg = "Client not initialized";
        return false;
    }

    m_client->DoActionFun(portName, funcName, inputValues, inputImages,
                          &outputValues, &outputImages, &retCode, &retMsg);
    return retCode == 0;
}

void TCP::cleanup() {
    if (m_client) {
        m_client->StopWork();
        m_client->FreeSDK();
        delete m_client;
        m_client = nullptr;
    }
}
