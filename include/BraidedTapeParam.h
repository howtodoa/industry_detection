#pragma once
#include <vector>

struct InSideParam
{
	bool al_core = 0;                //ѧϰʹ��
	float imgAngleNum = 0;               //ͼ����ת�Ƕ�
	float charAngleNum = 90;               //�ַ���ת�Ƕ�
	int buttonThresh = 120;                //����²��ֵ��
	int topThresh = 190;                   //����ϲ��ֵ��
	bool isColor = false;                  //�׹���ɫ�Ƿ��ж���trueΪ�ж�
};

struct OutSideParam
{
	std::stringstream m_VolumeStr, m_VoltageStr;//�ݻ��͵�ѹ���ַ���Ϣ
	bool m_VolumeOK = false, m_VoltageOK = false;               //�ݻ��͵�ѹ�ͺ�ƥ��
	bool m_PinOK = false;                                       //���״̬
	bool m_BreakOK = false;                                     //�Ƿ�����
	bool m_CaseColor = false;                                   //�׹���ɫ����
	bool m_CaseOK;                                              //�׹�״̬
	float m_PinDis = false;                                     //������ľ���
	float m_PinWidth;                                           //��Ŀ��
	float m_CaseHeigh;                                          //�׹ܿ��
	float m_Case2PinDis;                                        //��Ͳ����ļ��
	float m_CaseYSite;                                          //�׹�Y�������
	float m_TapeDis;                                            //�������
	float m_Case2TapeDis;                                       //�׹ܵ��������
};


struct InTopParam
{
	float angleNum;               //��ת�Ƕ�
	bool al_core;                //ѧϰʹ��
	bool isExproofExist;          //�������Ƿ���ڣ�true�Ǵ��ڣ���Ҫ���
};

struct OutTopParam
{
	bool m_FBFClassOKTop = false;                 //����������
	float m_NegAngleTop = -999.0;                 //�������ˮƽ�Ƕ�
	float m_CirNumTop = -999.0;                   //���������Բ��
	float m_RadiusTop = -999.0;                   //����������뾶
};

struct InPinParam
{
	float angleNum = 0;                               //��ת�Ƕ�
	float pinWidth = 0;                               //��ſ��
	float Diameter = 0;                               //��λ��ֱ��
	int thresh = 0;                                   //������ֵ������
	int PinThresh = 0;                               //�����������
	bool al_core=false;
};

struct OutPinParam
{
	float m_MaoCiArea;             //ë�̴���
	float m_DisCircle;          //��λԲ���
	float m_DisTopPin;          //������
	float m_DisButtomPin;       //��������
	float m_DisPinWid;          //�������
	float m_totalArea;             //����ȱ�������
	float m_DisTapeWid;            //�������
};



