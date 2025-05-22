#include "cameral.h"
#include <iostream>
Cameral::Cameral()
{


    //tcp=std::make_unique<TCP_Client>();
tcp = new TCP_Client();
    // 初始化 rangeParams 的默认值
    rangeParams.nePinLengthLower = 0.0f;
    rangeParams.nePinWidthLower = 0.0f;
    rangeParams.poPinLengthLower = 0.0f;
    rangeParams.piTotalLengthLower = 0.0f;
    rangeParams.poBendAngleMin = 0.0f;
    rangeParams.neBendAngleMin = 0.0f;
    rangeParams.nePinLengthUpper = 0.0f;
    rangeParams.nePinWidthUpper = 0.0f;
    rangeParams.poPinLengthUpper = 0.0f;
    rangeParams.piTotalLengthUpper = 0.0f;
    rangeParams.poBendAngleMax = 0.0f;
    rangeParams.neBendAngleMax = 0.0f;

    // 初始化 cameralParams 的默认值
    cameralParams.Cameral_Name = "";
    cameralParams.Cameral_SN = "";
    cameralParams.Cameral_IP = "";
    cameralParams.Cameral_Type = 0;
    cameralParams.Cameral_Station = 0;
    cameralParams.Cameral_AD = 0;
    cameralParams.Cameral_Gain = 0;
    cameralParams.Cameral_Exposure = 0;

    // 初始化 algoParams 的默认值
    algoParams.Algorithm_Name = "";
    algoParams.Value_Names.clear();
    algoParams.Value_Values.clear();
}

Cameral::~Cameral()
{
    //delete tcp;
    //tcp=nullptr;
   qDebug()<<"this is Cameral destroy";
}


bool Cameral::init(CommPorts port)
{
    return  this->tcp->initialize(port);
}

bool Cameral::doAction(const std::string& funcName,
                       const HValues& inputValues,
                       const HImages& inputImages,
                       HValues& outputValues,
                       HImages& outputImages,
                       int& retCode, std::string& retMsg)
{
    qDebug()<<"Cameral::doAction";
    return this->tcp->doAction(funcName,inputValues,inputImages,outputValues,outputImages,retCode,retMsg);
}

bool Cameral::start()
{
    return this->tcp->startWork();
}

void Cameral::stop()
{
     this->tcp->stopWork();
}

bool Cameral::RegsiterFunitFun(Callbackfunc func)
{
    return this->tcp->RegsiterFunitFun(func);
}

void Cameral::heartbeat_ui(HImages inputImages, HValues inputPrams,
                           HImages& outputImages, HValues& outputParams,
                           int& errcode, std::string& errmsg)
{
    qDebug()<<"enter";
    qDebug()<<"inputPrams.m_Values.size()"<<inputPrams.m_Values.size();
    if(inputPrams.m_Values.size()==0) qDebug()<<"error  heartbeat ";
        if(inputPrams.m_Values[0].S()=="hello")  outputParams.m_Values.push_back(inputPrams.m_Values[0]);
    else std::cout<<"error  heartbeat "<<std::endl;
    qDebug()<<"outputParams.m_Values[0].S()"<<outputParams.m_Values[0].S();
}
