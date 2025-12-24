#pragma once

class PCI1230;  // 前向声明

namespace PCI1230DLL
{
    static PCI1230* pci1230 = nullptr;

    class __declspec(dllexport) PCIControl
    {
    public:
        PCIControl();
        ~PCIControl();

        // 设备控制接口
        int Open(int cardId = 15);
        int Close(int cardId = 15);
        int ReadInput(int port, int cardId = 15);
        int WriteOutput(int port, int level, int cardId = 15);

        // 状态检查
        bool IsConnected() const;

        // 资源清理
        void Release();

    private:
        bool m_isConnected = false;
    };
    inline PCIControl& pci1230_inst() {
        static PCIControl instance;
        return instance;
    }
}