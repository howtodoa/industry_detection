#pragma once

#include <string>

class VC3000;  // 前向声明，实际实现包含在 cpp 中

namespace VC3000DLL
{
   static VC3000* vc3000 = nullptr;
   class __declspec(dllexport) PCIControl
    {
    public:
        // 初始化接口类
        PCIControl();
        ~PCIControl();
        // 初始化 WinIO 系统
        int InitialSystem();

        // 设置 PNP/NPN 模式
        int setPNP(unsigned int enable);

        // 打开串口
        int openseral(const std::string& portName);

        // 设置输出点电平
        int setoutput(int pointNumber, bool lowOrHigh);

        // 获取输入点电平
        int getinput(int inputPortNumber);

        // 控制光源
        int setlight(int portNumber,
            unsigned short lightValue, unsigned short continueTime,
            bool onOrOff, bool edgeUpOrDown);

        // 清理资源
        void Destroy();
    };
}
