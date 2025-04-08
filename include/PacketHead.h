#pragma once
#include <Windows.h>
#include <vector>
#include <string>

using namespace std;
typedef struct _HValue
{
	char m_Valuetype[64] = {0};
	char m_Value[64] = { 0 };
	int getdatelength()
	{
		return 128;
	}
}HValue;

typedef struct _HValues
{
	vector<HValue> m_Values;
	int nums;
	int getdatelength()
	{
		int length = 0;
		for (int i = 0; i < nums; i++)
		{
			length += m_Values[i].getdatelength();
		}
		return length + sizeof(int);
	}
}HValues;

typedef struct _ImageHead
{
	int width;
	int height;
	int channels;
	int getdatelength()
	{
		return width * height * channels;
	}
}ImageHeader;
typedef struct _HImage
{
	ImageHeader  imageHead;
	char* data;
	int getdatelength()
	{
		return imageHead.getdatelength() + sizeof(ImageHeader);
	}
}HImage;

typedef struct _HImages
{
	vector<HImage> m_Images;
	int nums;
	int getdatelength()
	{
		int length = 0;
		for (int i = 0; i < nums; i++)
		{
			length += m_Images[i].getdatelength();
		}
		return length + sizeof(int);
	}
}HImages;
//�󶨺������ƺͶ�Ӧ�Ļص�����
//�����ı�׼��ʽ functioName(inputImages,inputPrams,outputImages,outputPrams,errcode,errmsg);
//errcode��ʾ����ִ���Ƿ�ɹ���0��ʾ�ɹ���������ʾʧ�ܣ�errmsg��ʾ������Ϣ
typedef void (*FunctionTableStream)(HImages inputImages, HValues inputPrams, HImages& outputImages, HValues& outputPrams, int& errcode, string& errmsg);
typedef struct _Callbackfunc
{
	string funcname; //��������
	int inputImagesnums; //����ͼ�����
	int inputPramsnums; //�����������
	int outputImagesnums; //���ͼ�����
	int outputPramsnums; //�����������
	FunctionTableStream func; //����ָ��
}Callbackfunc;
//��ͻ���ͨ�Ŷ˿�
typedef struct _CommPorts
{
	int isActAsServer; //�Ƿ���Ϊ����ˣ�0��ʾ�ͻ��ˣ�1��ʾ�����
	std::string PortName;//�˿�����
	std::string localhost_IP;//����IP��ַ
	std::string remote_IP;//Զ��IP��ַ
	int nLocalPort;//���ؼ����˿ں�
	int nRemotePort;//Զ�����Ӷ˿ں�
	vector<Callbackfunc> m_Callbackfuncs; //�ص������б�
}CommPorts;



typedef struct _ImagePoints
{
	vector<HValues> m_Points;
	int nums;
	char name[64];
	float confidant;
}ImagePoints;

typedef struct _ResultData
{
	HImages m_Images;
	HValues m_Judges;
	vector<ImagePoints> m_ImagePoints;
	int Groupnums;
}ResultData;
const int TIME_OUT_COUNT = 20;
/*
*	��ͻ���ͨ��
*/

#define	OPERATE_CODE_CARELESS	((BYTE)0x00) 

#define HEAD_LABEL_SC_1 0xaa
#define HEAD_LABEL_SC_2 0x55

//////////////////////////////////////////////////////////////////////////
//�������
#define CAMERA_OPERATE_TYPE				OperateTypes(0x01,OPERATE_CODE_CARELESS) //��ʾ�������
#define CAMERA_COMMAND_SeT					OperateTypes(0x01,0x01) //���commandͨ��-����
#define CAMERA_REQUEST_Get					OperateTypes(0x01,0x02) //���requestͨ��-����

#define CAMERA_COMMAND_Set_R				OperateTypes(0x01,0x11) //���commandͨ��-����
#define CAMERA_REQUEST_Get_R				OperateTypes(0x01,0x12) //���requestͨ��-����0x0

//////////////////////////////////////////////////////////////////////////
//IO������
#define IO_OPERATE_TYPE				OperateTypes(0x20,OPERATE_CODE_CARELESS) //��ʾ�źŽӿ�ƽ̨����
#define IO_COMMAND_GPIO_Set				OperateTypes(0x20,0x01) //set
#define IO_COMMAND_GPIO_Get				OperateTypes(0x20,0x02) //get 
#define IO_COMMAND_LIGHT_Set				OperateTypes(0x20,0x03) //set 
#define IO_COMMAND_LIGHT_Get				OperateTypes(0x20,0x04) //get 
#define IO_COMMAND_IOBOARD_Set				OperateTypes(0x20,0x05) //set 
#define IO_COMMAND_IOBOARD_Get				OperateTypes(0x20,0x06) //get 
#define IO_COMMAND_PCIE_Set				OperateTypes(0x20,0x07) //set 
#define IO_COMMAND_PCIE_Get				OperateTypes(0x20,0x08) //get 

#define IO_COMMAND_GPIO_Set_R				OperateTypes(0x20,0x11) //set 
#define IO_COMMAND_GPIO_Get_R				OperateTypes(0x20,0x12) //get 
#define IO_COMMAND_LIGHT_Set_R				OperateTypes(0x20,0x13) //set 
#define IO_COMMAND_LIGHT_Get_R				OperateTypes(0x20,0x14) //get 
#define IO_COMMAND_IOBOARD_Set_R				OperateTypes(0x20,0x15) //set 
#define IO_COMMAND_IOBOARD_Get_R				OperateTypes(0x20,0x16) //get 
#define IO_COMMAND_PCIE_Set_R				OperateTypes(0x20,0x17) //set 
#define IO_COMMAND_PCIE_Get_R				OperateTypes(0x20,0x18) //get 

//PLC����
#define PLC_OPERATE_TYPE				OperateTypes(0x30,OPERATE_CODE_CARELESS) //��ʾPLC����
#define PLC_COMMAND_Set					OperateTypes(0x30,0x01) //PLC����ͨ��-����
#define PLC_COMMAND_Get					OperateTypes(0x30,0x01) //PLC����ͨ��-����

#define PLC_COMMAND_Set_R				OperateTypes(0x30,0x11) //PLC����ͨ��-����
#define PLC_COMMAND_Get_R				OperateTypes(0x30,0x11) //PLC����ͨ��-����

//ϵͳ��������
#define SYS_PARAM_OPERATE_TYPE			OperateTypes(0x40,OPERATE_CODE_CARELESS) //��ʾϵͳ��������
#define SYS_PARAM_SET_PARAM				OperateTypes(0x40,0x01) //����ϵͳ����
#define SYS_PARAM_GET_PARAM				OperateTypes(0x40,0x02) //��ȡϵͳ����	
#define SYS_PARAM_SET_PARAM_R			OperateTypes(0x40,0x11) //����ϵͳ�������
#define SYS_PARAM_GET_PARAM_R			OperateTypes(0x40,0x12) //��ȡϵͳ�������

//�㷨��������
#define ALG_PARAM_OPERATE_TYPE			OperateTypes(0x50,OPERATE_CODE_CARELESS) //��ʾ�㷨��������
#define ALG_PARAM_SET_PARAM				OperateTypes(0x50,0x01) //�����㷨����
#define ALG_PARAM_GET_PARAM				OperateTypes(0x50,0x02) //��ȡ�㷨����
#define ALG_PARAM_SET_PARAM_R			OperateTypes(0x50,0x11) //�����㷨�������
#define ALG_PARAM_GET_PARAM_R			OperateTypes(0x50,0x12) //��ȡ�㷨�������

//���ݿ��������
#define DB_PARAM_OPERATE_TYPE			OperateTypes(0x60,OPERATE_CODE_CARELESS) //��ʾ���ݿ��������
#define DB_PARAM_SET_PARAM				OperateTypes(0x60,0x01) //�������ݿ����
#define DB_PARAM_GET_PARAM				OperateTypes(0x60,0x02) //��ȡ���ݿ����
#define DB_PARAM_SET_PARAM_R			OperateTypes(0x60,0x11) //�������ݿ�������
#define DB_PARAM_GET_PARAM_R			OperateTypes(0x60,0x12) //��ȡ���ݿ�������
//////////////////////////////////////////////////////////////////////////
//������ݷ���
#define RES_DATA_OPERATE_TYPE			OperateTypes(0x80,OPERATE_CODE_CARELESS) //��ʾ������ݲ���
#define RES_DATA_SEND_DATA				OperateTypes(0x80,0x01) //���ͽ������

//������Ϣ����
#define ALARM_MSG_OPERATE_TYPE			OperateTypes(0x90,OPERATE_CODE_CARELESS) //��ʾ������Ϣ����
#define ALARM_MSG_RECEIVE_MSG			OperateTypes(0x90,0x01) //���ձ�����Ϣ
#define ALARM_MSG_RECEIVE_MSG			OperateTypes(0x90, 0x02) //���ձ�����Ϣ
#define ALARM_MSG_RECEIVE_MSG			OperateTypes(0x90, 0x03) //���ձ�����Ϣ
#define ALARM_MSG_RECEIVE_MSG			OperateTypes(0x90, 0x04) //���ձ�����Ϣ
#define ALARM_MSG_RECEIVE_MSG			OperateTypes(0x90, 0x05) //���ձ�����Ϣ
#define ALARM_MSG_RECEIVE_MSG			OperateTypes(0x90, 0x06) //���ձ�����Ϣ
//////////////////////////////////////////////////////////////////////////

typedef struct OperateTypes
{
	BYTE	operate_code_1;
	BYTE	operate_code_2;

	OperateTypes()
	{
		operate_code_1 = OPERATE_CODE_CARELESS;
		operate_code_2 = OPERATE_CODE_CARELESS;
	}

	OperateTypes(BYTE code_1, BYTE code_2)
	{
		operate_code_1 = code_1;
		operate_code_2 = code_2;
	}

	bool operator ==(const OperateTypes& tempTypes)
	{
		if ((operate_code_1 == tempTypes.operate_code_1) && ((operate_code_2 == tempTypes.operate_code_2) || (OPERATE_CODE_CARELESS == tempTypes.operate_code_2) || (operate_code_2 == OPERATE_CODE_CARELESS)))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}OperateTypes;

typedef struct _sc_pack_head
{
	unsigned char label_1;
	unsigned char label_2;

	OperateTypes operate_type;
	typedef union _UserDefine
	{
		char user_dummy[64];
		string func_name;
	}UserDefine;
	
	UserDefine user_define;
	unsigned char inputParamsnums;
	unsigned char inputImagesnums;
	unsigned long data_len;
}SC_PackHead;
