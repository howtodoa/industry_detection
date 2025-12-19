#pragma once
#include <string>
#include <windows.h>

// RAII 辅助类，用于自动管理锁的生命周期，确保安全
class CriticalSectionLock {
public:
    explicit CriticalSectionLock(CRITICAL_SECTION& cs) : m_cs(cs) {
        EnterCriticalSection(&m_cs);
    }
    ~CriticalSectionLock() {
        LeaveCriticalSection(&m_cs);
    }
private:
    // 禁止拷贝和赋值，确保锁的唯一性
    CriticalSectionLock(const CriticalSectionLock&) = delete;
    CriticalSectionLock& operator=(const CriticalSectionLock&) = delete;

    CRITICAL_SECTION& m_cs;
};

class VC3000H;  // 前向声明

namespace PCI
{
    class __declspec(dllexport) PCIControl
    {
    public:
        PCIControl();
        ~PCIControl();

        int InitialSystem();
      //  int SaveParam();
        int setPNP(unsigned int enable, const char* mode);
        int openseral(const char* portName);
        int setoutput(int pointNumber, bool lowOrHigh);
        int getinput(int inputPortNumber);
        int setInputMode(int pointNumber, bool level, unsigned int delayTime, unsigned int glitchTime);
        int setOutputMode(int pointNumber, bool level, unsigned short durationMs);
        int setlight(int portNumber,
            unsigned short lightValue, unsigned short continueTime,
            int lightState, bool edgeUpOrDown, int LightTriggerSource);
        void Destroy();

    private:
        VC3000H* vc3000h = nullptr;
        CRITICAL_SECTION m_critSec;
    };

    inline PCIControl& pci() {
        static PCIControl instance;
        return instance;
    }
}