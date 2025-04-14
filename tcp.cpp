#include "TCP.h"

TCP::TCP()
{
    // 在构造函数中初始化 ClientOperation 对象
    m_client = new Mz_ClientControl::ClientOperation();
}

TCP::~TCP()
{
    // 在析构函数中清理资源
    cleanup();
}

bool TCP::initialize(const std::vector<CommPorts>& ports)
{
    if (ports.empty()) {
        return false; // 如果没有端口配置，返回 false
    }

    // 保存端口配置
    m_ports = ports;

    // 初始化 SDK 并启动工作
    for (const auto& port : m_ports) {
        m_client->InitSDK(port);  // 初始化每个端口
    }

    m_client->StartWork();  // 启动工作

    return true;
}

void TCP::cleanup()
{
    // 停止工作并释放 SDK
    m_client->StopWork();
    m_client->FreeSDK();

    // 清理 ClientOperation 对象
    delete m_client;
    m_client = nullptr;

}

bool TCP::startWork()
{
    m_client->StartWork();
    return true;
}

void TCP::stopWork()
{
    m_client->StopWork();
}

bool TCP::doAction(const std::string& portName, const std::string& funcName,
                   const HValues& inputValues, const HImages& inputImages,
                   HValues& outputValues, HImages& outputImages,
                   int& retCode, std::string& retMsg)
{
    // 假设 commPorts 由 portName 查找并获取
    CommPorts port;
    for (const auto& p : m_ports) {
        if (p.PortName == portName) {
            port = p;
            break;
        }
    }

    if (port.PortName.empty()) {
        retMsg = "Port not found!";
        retCode = -1;
        return false;  // 如果没有找到对应的端口，返回 false
    }

    // 调用 ClientOperation 的 DoActionFun 方法
    m_client->DoActionFun(port, funcName, inputValues, inputImages, &outputValues, &outputImages, &retCode, &retMsg);

    return retCode == 0;  // 如果返回码为 0，表示操作成功
}

bool TCP::registerLocalFunction(const Callbackfunc& func)
{
    m_client->RegsiterFunitFun(func);
    return true;
}

bool TCP::getRemoteFunctionList(const std::string& portName, std::vector<Callbackfunc>& funList)
{
    // 假设 commPorts 由 portName 查找并获取
    CommPorts port;
    for (const auto& p : m_ports) {
        if (p.PortName == portName) {
            port = p;
            break;
        }
    }

    if (port.PortName.empty()) {
        return false;  // 如果没有找到对应的端口，返回 false
    }

    m_client->GetFunList_Remote(port, funList);
    return true;
}

bool TCP::getRemoteCommPorts(std::vector<CommPorts>& commports)
{
    m_client->GetRemoteCommports(commports);
    return true;
}
