#pragma once

#include <string>

class VC3000;  // ǰ��������ʵ��ʵ�ְ����� cpp ��

namespace VC3000DLL
{
   static VC3000* vc3000 = nullptr;
   class __declspec(dllexport) PCIControl
    {
    public:
        // ��ʼ���ӿ���
        PCIControl();
        ~PCIControl();
        // ��ʼ�� WinIO ϵͳ
        int InitialSystem();

        // ���� PNP/NPN ģʽ
        int setPNP(unsigned int enable);

        // �򿪴���
        int openseral(const std::string& portName);

        // ����������ƽ
        int setoutput(int pointNumber, bool lowOrHigh);

        // ��ȡ������ƽ
        int getinput(int inputPortNumber);

        // ���ƹ�Դ
        int setlight(int portNumber,
            unsigned short lightValue, unsigned short continueTime,
            bool onOrOff, bool edgeUpOrDown);

        // ������Դ
        void Destroy();
    };
}
