#pragma once
#include <string>

namespace VC5000DLL
{
    class PCIControl; // 前向声明内部逻辑类

    class __declspec(dllexport) VC5000
    {
    public:
        VC5000();
        ~VC5000();

        // 系统初始化：内部对应 MV_IOC_Create, Initialize, Open
        int InitialSystem();

        // 设置输出极性：0-NPN, 1-PNP
        int setPNP(unsigned int enable);

        // 打开通讯口 (适配模式，VC5000 SDK 通常在 Open 内部自动处理)
        int openseral(const char* portName);

        // 设置输出：pointNumber(1-16/32), lowOrHigh(true-高, false-低)
        int setoutput(int pointNumber, bool lowOrHigh);

        // 获取输入：inputNumber(1-N)
        int getinput(int inputNumber);

        // 光源控制
        int setlight(int portNumber,
            unsigned short lightValue, unsigned short continueTime,
            bool onOrOff, bool edgeUpOrDown);

        void Destroy();

    private:
        PCIControl* m_pciCtrl = nullptr;
    public:
        bool m_isOpenCom = false;
    };

    // 单例模式访问，保持与 3000 模式一致
    inline VC5000& vc5000_inst() {
        static VC5000 instance;
        return instance;
    }
}