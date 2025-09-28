#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

//#include <QObject>
#include <string>
#include <mutex>
#include "MZ_ClientControl.h"

class TCP_Client
{
    //Q_OBJECT

public:

    inline static TCP_Client* tcp1 = nullptr;
   inline static TCP_Client* tcp2 = nullptr;
   inline static TCP_Client* tcp3 = nullptr;
   inline static TCP_Client* tcp4 = nullptr;
    inline static TCP_Client* tcp5 = nullptr;
   inline static TCP_Client* tcp6 = nullptr;
    explicit TCP_Client();
   // explicit TCP_Client(QObject* parent = nullptr);
    ~TCP_Client();

    bool initialize(const CommPorts& port);        // 初始化绑定端口
    bool startWork();                              // 启动
    void stopWork();                               // 停止
    bool doAction(const std::string& funcName,
                  const HValues& inputValues,
                  const HImages& inputImages,
                  HValues& outputValues,
                  HImages& outputImages,
                  int& retCode, std::string& retMsg);  // 执行功能
   bool RegsiterFunitFun(Callbackfunc func);
   bool GetFunList_Remote(CommPorts portname,vector<Callbackfunc>& funList);
   bool GetRemoteCommports(vector<CommPorts>& commports);

    CommPorts m_port;
    bool m_isInitialized = false;
    bool m_isRunning = false;
  Mz_ClientControl::ClientOperation* m_clientOp = nullptr;
    private:
    std::mutex m_mutex;

};

#endif // TCP_CLIENT_H
