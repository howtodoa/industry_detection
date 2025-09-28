#pragma once
#include <string>

class VC3000;  // Ç°ÏòÉùÃ÷

namespace VC3000DLL
{
    static VC3000* vc3000 = nullptr;

    class __declspec(dllexport) PCIControl
    {
    public:
        PCIControl();
        ~PCIControl();
        int InitialSystem();
        int setPNP(unsigned int enable);
        int openseral(const std::string& portName);
        int setoutput(int pointNumber, bool lowOrHigh);
        int getinput(int inputNumber);
        int setlight(int portNumber,
            unsigned short lightValue, unsigned short continueTime,
            bool onOrOff, bool edgeUpOrDown);
        void Destroy();

    private:
    };
    inline PCIControl& pci() {
        static PCIControl instance;
        return instance;
    }
}