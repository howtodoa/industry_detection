#ifndef TCP_H
#define TCP_H

#include "MZ_ClientControl.h"
#include <QObject>
#include <QString>
#include <vector>

class TCP:public QObject {
public:
    TCP();
    ~TCP();

    bool initialize(const std::vector<CommPorts>& ports);
    bool doAction(const std::string& portName, const std::string& funcName,
                  const HValues& inputValues, const HImages& inputImages,
                  HValues& outputValues, HImages& outputImages,
                  int& retCode, std::string& retMsg);
    void cleanup();

private:
    Mz_ClientControl::ClientOperation* m_client;
};
#endif // TCP_H
