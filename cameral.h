#ifndef CAMERAL_H
#define CAMERAL_H

//#include <QObject>
#include "public.h"
#include "tcp_client.h"
#include<deque>
#include<atomic>
#include<memory>
class Cameral
{
    //Q_OBJECT
public:
  //  inline static vector<std::shared_ptr<Cameral>> Cams;
    inline static vector<Cameral*> Cams;
   // explicit Cameral(QObject *parent = nullptr);
    Cameral();
    ~Cameral();
    bool init(CommPorts port);
    bool start();
    void stop();
    bool RegsiterFunitFun(Callbackfunc func);
    bool doAction(const std::string& funcName,
                  const HValues& inputValues,
                  const HImages& inputImages,
                  HValues& outputValues,
                  HImages& outputImages,
                  int& retCode, std::string& retMsg);
    // 三个结构体的成员变量
    RangePara rangeParams;       // 范围参数
    CameralPara cameralParams;   // 相机参数
    AlgorithmPara algoParams;    // 算法参数

    TCP_Client  * tcp=nullptr;
 //   std::unique_ptr<TCP_Client> tcp;
    std::deque<HImage> deque;
    std::mutex mutex;
    std::atomic<bool> photoflag;

    void heartbeat_ui(HImages inputImages, HValues inputPrams,
                      HImages& outputImages, HValues& outputParams,
                      int& errcode, std::string& errmsg);

};

#endif // CAMERAL_H
