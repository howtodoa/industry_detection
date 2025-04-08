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
//绑定函数名称和对应的回调函数
//函数的标准格式 functioName(inputImages,inputPrams,outputImages,outputPrams,errcode,errmsg);
//errcode表示函数执行是否成功，0表示成功，其他表示失败，errmsg表示错误信息
typedef void (*FunctionTableStream)(HImages inputImages, HValues inputPrams, HImages& outputImages, HValues& outputPrams, int& errcode, string& errmsg);
typedef struct _Callbackfunc
{
	string funcname; //函数名称
	int inputImagesnums; //输入图像个数
	int inputPramsnums; //输入参数个数
	int outputImagesnums; //输出图像个数
	int outputPramsnums; //输出参数个数
	FunctionTableStream func; //函数指针
}Callbackfunc;
//与客户端通信端口
typedef struct _CommPorts
{
	int isActAsServer; //是否作为服务端，0表示客户端，1表示服务端
	std::string PortName;//端口名称
	std::string localhost_IP;//本机IP地址
	std::string remote_IP;//远程IP地址
	int nLocalPort;//本地监听端口号
	int nRemotePort;//远程连接端口号
	vector<Callbackfunc> m_Callbackfuncs; //回调函数列表
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
*	与客户端通信
*/

#define	OPERATE_CODE_CARELESS	((BYTE)0x00) 

#define HEAD_LABEL_SC_1 0xaa
#define HEAD_LABEL_SC_2 0x55

//////////////////////////////////////////////////////////////////////////
//相机操作
#define CAMERA_OPERATE_TYPE				OperateTypes(0x01,OPERATE_CODE_CARELESS) //表示相机操作
#define CAMERA_COMMAND_SeT					OperateTypes(0x01,0x01) //相机command通信-发送
#define CAMERA_REQUEST_Get					OperateTypes(0x01,0x02) //相机request通信-发送

#define CAMERA_COMMAND_Set_R				OperateTypes(0x01,0x11) //相机command通信-接收
#define CAMERA_REQUEST_Get_R				OperateTypes(0x01,0x12) //相机request通信-接收0x0

//////////////////////////////////////////////////////////////////////////
//IO卡操作
#define IO_OPERATE_TYPE				OperateTypes(0x20,OPERATE_CODE_CARELESS) //表示信号接口平台操作
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

//PLC操作
#define PLC_OPERATE_TYPE				OperateTypes(0x30,OPERATE_CODE_CARELESS) //表示PLC操作
#define PLC_COMMAND_Set					OperateTypes(0x30,0x01) //PLC命令通信-发送
#define PLC_COMMAND_Get					OperateTypes(0x30,0x01) //PLC命令通信-发送

#define PLC_COMMAND_Set_R				OperateTypes(0x30,0x11) //PLC命令通信-接收
#define PLC_COMMAND_Get_R				OperateTypes(0x30,0x11) //PLC命令通信-接收

//系统参数操作
#define SYS_PARAM_OPERATE_TYPE			OperateTypes(0x40,OPERATE_CODE_CARELESS) //表示系统参数操作
#define SYS_PARAM_SET_PARAM				OperateTypes(0x40,0x01) //设置系统参数
#define SYS_PARAM_GET_PARAM				OperateTypes(0x40,0x02) //获取系统参数	
#define SYS_PARAM_SET_PARAM_R			OperateTypes(0x40,0x11) //设置系统参数结果
#define SYS_PARAM_GET_PARAM_R			OperateTypes(0x40,0x12) //获取系统参数结果

//算法参数操作
#define ALG_PARAM_OPERATE_TYPE			OperateTypes(0x50,OPERATE_CODE_CARELESS) //表示算法参数操作
#define ALG_PARAM_SET_PARAM				OperateTypes(0x50,0x01) //设置算法参数
#define ALG_PARAM_GET_PARAM				OperateTypes(0x50,0x02) //获取算法参数
#define ALG_PARAM_SET_PARAM_R			OperateTypes(0x50,0x11) //设置算法参数结果
#define ALG_PARAM_GET_PARAM_R			OperateTypes(0x50,0x12) //获取算法参数结果

//数据库参数操作
#define DB_PARAM_OPERATE_TYPE			OperateTypes(0x60,OPERATE_CODE_CARELESS) //表示数据库参数操作
#define DB_PARAM_SET_PARAM				OperateTypes(0x60,0x01) //设置数据库参数
#define DB_PARAM_GET_PARAM				OperateTypes(0x60,0x02) //获取数据库参数
#define DB_PARAM_SET_PARAM_R			OperateTypes(0x60,0x11) //设置数据库参数结果
#define DB_PARAM_GET_PARAM_R			OperateTypes(0x60,0x12) //获取数据库参数结果
//////////////////////////////////////////////////////////////////////////
//结果数据发送
#define RES_DATA_OPERATE_TYPE			OperateTypes(0x80,OPERATE_CODE_CARELESS) //表示结果数据操作
#define RES_DATA_SEND_DATA				OperateTypes(0x80,0x01) //发送结果数据

//报警信息接收
#define ALARM_MSG_OPERATE_TYPE			OperateTypes(0x90,OPERATE_CODE_CARELESS) //表示报警信息操作
#define ALARM_MSG_RECEIVE_MSG			OperateTypes(0x90,0x01) //接收报警信息
#define ALARM_MSG_RECEIVE_MSG			OperateTypes(0x90, 0x02) //接收报警信息
#define ALARM_MSG_RECEIVE_MSG			OperateTypes(0x90, 0x03) //接收报警信息
#define ALARM_MSG_RECEIVE_MSG			OperateTypes(0x90, 0x04) //接收报警信息
#define ALARM_MSG_RECEIVE_MSG			OperateTypes(0x90, 0x05) //接收报警信息
#define ALARM_MSG_RECEIVE_MSG			OperateTypes(0x90, 0x06) //接收报警信息
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
