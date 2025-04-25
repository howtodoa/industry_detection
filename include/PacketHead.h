#pragma once
#include <Windows.h>
#include <vector>
#include <string>

using namespace std;
typedef struct _HValue
{
	char m_Valuetype[32] = {0};
	char m_Value[64] = { 0 };
	char* getDate()
	{
		return &m_Valuetype[0];
	}
	int getdatelength()
	{
		return sizeof(m_Valuetype) + sizeof(m_Value);
	}
	int I()
	{
		return atoi(m_Value);
	}
	int D()
	{
		return atof(m_Value);
	}
	string S()
	{
		return m_Value;
	}
	_HValue()
	{
		strcpy_s(m_Valuetype, "null");
		strcpy_s(m_Value, "");
	}
	_HValue(int value)
	{
		strcpy_s(m_Valuetype, "int");
		sprintf_s(m_Value, "%d", value);
	}
	_HValue(double value)
	{
		strcpy_s(m_Valuetype, "double");
		sprintf_s(m_Value, "%f", value);
	}
	_HValue(const char* value)
	{
		strcpy_s(m_Valuetype, "string");
		strcpy_s(m_Value, value);
	}
	_HValue(const _HValue& value)
	{
		strcpy_s(m_Valuetype, value.m_Valuetype);
		strcpy_s(m_Value, value.m_Value);
	}
	_HValue operator=(const _HValue& value)
	{
		strcpy_s(m_Valuetype, value.m_Valuetype);
		strcpy_s(m_Value, value.m_Value);
		return *this;
	}
	_HValue operator=(const char* value)
	{
		strcpy_s(m_Valuetype, "string");
		strcpy_s(m_Value, value);
		return *this;
	}
	_HValue operator=(int value)
	{
		strcpy_s(m_Valuetype, "int");
		sprintf_s(m_Value, "%d", value);
		return *this;
	}
	_HValue operator=(double value)
	{
		strcpy_s(m_Valuetype, "double");
		sprintf_s(m_Value, "%f", value);
		return *this;
	}
}HValue;

typedef struct _HValues
{
	vector<HValue> m_Values;
	char *getDate()
	{
		if(m_Values.size() == 0)
			return NULL;
		return (char*)m_Values[0].getDate();
	}
	int getdatelength()
	{
		int length = sizeof(HValue) * m_Values.size();
		return length;
	}
	int getValueNums()
	{
		return m_Values.size();
	}
	_HValues()
	{
	}
	~_HValues()
	{
		m_Values.clear();
	}
	_HValues(const int value)
	{
		HValue value1 = value;
		m_Values.push_back(value1);
	}
	_HValues(const double value)
	{
		HValue value1;
		value1 = value;
		m_Values.push_back(value1);
	}
	_HValues(const char* value)
	{
		HValue value1;
		value1 = value;
		m_Values.push_back(value1);
	}
	_HValues(const vector<HValue>& values)
	{
		for (int i = 0; i < values.size(); i++)
		{
			m_Values.push_back(values[i]);
		}
	}
	_HValues operator=(const _HValues& values)
	{
		m_Values.clear();
		for (int i = 0; i < values.m_Values.size(); i++)
		{
			m_Values.push_back(values.m_Values[i]);
		}
		return *this;
	}
	_HValues operator=(const int value)
	{
		m_Values.clear();
		HValue value1;
		value1= value;
		m_Values.push_back(value1);
		return *this;
	}
	_HValues operator=(const double value)
	{
		m_Values.clear();
		HValue value1;
		value1 = value;
		m_Values.push_back(value1);
		return *this;
	}
	_HValues operator=(const char* value)
	{
		m_Values.clear();
		HValue value1;
		value1 = value;
		m_Values.push_back(value1);
		return *this;
	}
	_HValues operator=(const vector<HValue>& values)
	{
		m_Values.clear();
		for (int i = 0; i < values.size(); i++)
		{
			m_Values.push_back(values[i]);
		}
		return *this;
	}
}HValues;

typedef struct _ImageHead
{
	int width = 0;
	int height = 0;
	int channels = 0;
	int length = 0;
	void clear()
	{
		width = 0;
		height = 0;
		channels = 0;
		length = 0;
	}
	_ImageHead()
	{
		clear();
	}
	_ImageHead(const _ImageHead& image)
	{
		width = image.width;
		height = image.height;
		channels = image.channels;
		length = image.length;
	}
	_ImageHead(int width, int height, int channels, int length)
	{
		this->width = width;
		this->height = height;
		this->channels = channels;
		this->length = length;
	}
}ImageHeader;
typedef struct _HImage
{
	ImageHeader  imageHead;
	unsigned char* data = NULL;
	int getdatelength()
	{
		return imageHead.length + sizeof(ImageHeader);
	}
	void Rlease()
	{
		imageHead.clear();
		if (data != NULL)
		{
			delete data;
			data = NULL;
		}
	}
	_HImage()
	{
		data = NULL;
	}
	_HImage(const _HImage& image)
	{
		imageHead = image.imageHead;
		if (image.data != NULL)
		{
			data = new unsigned char[image.imageHead.length];
			if (data != NULL)
			{
				memcpy(data, image.data, image.imageHead.length);
			}
		}
	}
	~_HImage()
	{
		if (data != NULL)
		{
			delete data;
			data = NULL;
		}
		imageHead.clear();
	}
	_HImage(int width, int height, int channels, int length, unsigned char* inputdata)
	{
		imageHead.width = width;
		imageHead.height = height;
		imageHead.channels = channels;
		imageHead.length = length;
		data = new unsigned char[length];
		if (data != NULL)
		{
			memcpy(data, inputdata, length);
		}
	}
	_HImage(char* inputdata, int length)
	{
		memcpy(&imageHead, inputdata, sizeof(ImageHeader));
		data = new unsigned char[imageHead.length];
		if (data != NULL)
		{
			memcpy(data, inputdata + sizeof(ImageHeader), imageHead.length);
		}
	}
	_HImage operator=(const _HImage& image)
	{
		imageHead = image.imageHead;
		if (data != NULL)
		{
			delete[] data;
			data = NULL;
		}
		if (image.data != NULL)
		{
			data = new unsigned char[image.imageHead.length];
			if (data != NULL)
			{
				memcpy(data, image.data, image.imageHead.length);
			}
		}
		return *this;
	}
}HImage;

typedef struct _HImages
{
	vector<HImage> m_Images;
	void Relase()
	{
		for (int i = 0; i < m_Images.size(); i++)
		{
			m_Images[i].Rlease();
		}
		m_Images.clear();
	}
	_HImages()
	{

	}
	~_HImages()
	{
		for (int i = 0; i < m_Images.size(); i++)
		{
			m_Images[i].Rlease();
		}
		m_Images.clear();
	}
	_HImages(const _HImages& images)
	{
		for (int i = 0; i < images.m_Images.size(); i++)
		{
			m_Images.push_back(images.m_Images[i]);
		}
	}
	_HImages operator=(const _HImages& images)
	{
		m_Images.clear();
		for (int i = 0; i < images.m_Images.size(); i++)
		{
			m_Images.push_back(images.m_Images[i]);
		}
		return *this;
	}
	int getdatelength()
	{
		int length = 0;
		for (int i = 0; i < m_Images.size(); i++)
		{
			length += m_Images[i].getdatelength();
		}
		return length;
	}
	int getImageNums()
	{
		return m_Images.size();
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
//存储IP地址和端口号
typedef struct _IPPort
{
	std::string IP;
	int Port;
	_IPPort()
	{
		IP = "";
		Port = 0;
	}
	_IPPort(const std::string& ip, int port)
	{
		IP = ip;
		Port = port;
	}
	void init()
	{
		IP = "";
		Port = 0;
	}
	_IPPort operator=(const _IPPort& ip_port)
	{
		IP = ip_port.IP;
		Port = ip_port.Port;
		return *this;
	}
}IPPort;
//与客户端通信端口
typedef struct _CommPorts
{
	int isActAsServer; //是否作为服务端，0表示客户端，1表示服务端
	std::string PortName;//注册端口名称
	IPPort localhost_IP;//本机IP地址
	void init()
	{
		isActAsServer = -1;
		PortName = "";
		localhost_IP.init();
	}
	_CommPorts()
	{
		init();
	}
	_CommPorts(int isServer, const std::string& portName, const IPPort& localhost_ip)
	{
		isActAsServer = isServer;
		PortName = portName;
		localhost_IP = localhost_ip;
	}
	_CommPorts(const std::string ip, const int port)
	{
		isActAsServer = 0;
		PortName = "";
		localhost_IP.init();
		localhost_IP.IP = ip;
		localhost_IP.Port = port;
	}
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

typedef union _UserDefine
{
	char user_dummy[64];
	_UserDefine()
	{
		memset(user_dummy, 0, 64);
	}
	_UserDefine& operator = (const string& input)
	{
		memcpy(user_dummy, input.c_str(), input.length());
		return *this;
	}
	string toStr()
	{
		return string(user_dummy);
	}
}UserDefine;

typedef struct _sc_pack_head
{
	unsigned char label_1;
	unsigned char label_2;

	OperateTypes operate_type;
	UserDefine user_define;
	unsigned char inputParamsnums;
	unsigned char inputImagesnums;
	unsigned long data_len;//数据长度

	char *GetData()
	{
		return (char*)&label_1;
	}
	void init()
	{
		label_1 = HEAD_LABEL_SC_1;
		label_2 = HEAD_LABEL_SC_2;
		operate_type.operate_code_1 = OPERATE_CODE_CARELESS;
		operate_type.operate_code_2 = OPERATE_CODE_CARELESS;
		inputParamsnums = 0;
		inputImagesnums = 0;
		data_len = 0;
	}
	_sc_pack_head()
	{
		label_1 = HEAD_LABEL_SC_1;
		label_2 = HEAD_LABEL_SC_2;
		operate_type.operate_code_1 = OPERATE_CODE_CARELESS;
		operate_type.operate_code_2 = OPERATE_CODE_CARELESS;
		inputParamsnums = 0;
		inputImagesnums = 0;
		data_len = 0;
	}
	_sc_pack_head(OperateTypes inputOper, string inputDef, int inputParams, int inputImages)
	{
		label_1 = HEAD_LABEL_SC_1;
		label_2 = HEAD_LABEL_SC_2;
		operate_type = inputOper;
		user_define = inputDef;
		inputParamsnums = inputParams;
		inputImagesnums = inputImages;
		data_len = 0;
	}
	_sc_pack_head& operator = (const _sc_pack_head& input)
	{
		if (this != &input)
		{
		label_1 = input.label_1;
		label_2 = input.label_2;
		operate_type = input.operate_type;
		user_define = input.user_define;
		inputParamsnums = input.inputParamsnums;
		inputImagesnums = input.inputImagesnums;
		data_len = input.data_len;
		}
		return *this;
	}
}SC_PackHead;

typedef struct _sc_pack_data
{
	int data_len = 0;
	char* buffer = NULL;
	void init()
	{
		data_len = 0;
		buffer = NULL;
	}
	_sc_pack_data()
	{
		int data_len = 0;
		buffer = NULL;
	}
	_sc_pack_data(const _sc_pack_data& aPacket, int aPacketLength)
	{
		buffer = new char[aPacketLength];
		memcpy(buffer, aPacket.buffer, aPacketLength);
	}
	_sc_pack_data(char* inputbuffer, int inputbufferLength)
	{
		buffer = new char[inputbufferLength];
		data_len = inputbufferLength;
		memcpy(buffer, inputbuffer, inputbufferLength);
	}
	_sc_pack_data(HValues inputValues, HImages inputImages)
	{
		buffer = new char[inputValues.getdatelength() + inputImages.getdatelength()];
		memcpy(buffer, inputValues.m_Values[0].m_Value, inputValues.getdatelength());
		memcpy(buffer + inputValues.getdatelength(), inputImages.m_Images[0].data, inputImages.getdatelength());
	}
	~_sc_pack_data()
	{
		if (buffer)
		{
			delete[] buffer;
			buffer = NULL;
		}
	}
	_sc_pack_data& operator = (const _sc_pack_data& input)
	{
		if (this != &input)
		{
			if (buffer)
			{
				delete[] buffer;
				buffer = NULL;
			}
			buffer = new char[input.data_len];
			memcpy(buffer, input.buffer, input.data_len);
		}
		return *this;
	}
}SC_PackData;

typedef struct _SC_PacketData
{
	SC_PackHead head;
	//先放变量，再放图片
	SC_PackData data;

	_SC_PacketData()
	{
		head.init();
		data.init();
	}
	_SC_PacketData(SC_PackHead inputHead, SC_PackData inputData)
	{
		head = inputHead;
		data = inputData;
	}

	_SC_PacketData(const _SC_PacketData& aPacket)
	{
		this->head = aPacket.head;
		this->data.buffer = new char[aPacket.data.data_len];
		this->data.data_len = aPacket.data.data_len;
		memcpy(this->data.buffer, aPacket.data.buffer, aPacket.head.data_len);
	}
	_SC_PacketData operator = (const _SC_PacketData& aPacket)
	{
		this->head = aPacket.head;
		this->data.buffer = new char[aPacket.data.data_len];
		this->data.data_len = aPacket.data.data_len;
		memcpy(this->data.buffer, aPacket.data.buffer, aPacket.head.data_len);
		return *this;
	}
	_SC_PacketData(string funcName,HImages inputImages, HValues inputPrams)
	{
		head.user_define = funcName;
		head.inputImagesnums = inputImages.getImageNums();
		head.inputParamsnums = inputPrams.getValueNums();
		int Imagesdatasize = inputImages.getdatelength();
		int Paramsdatasize = inputPrams.getdatelength();
		head.data_len = Imagesdatasize + Paramsdatasize;
		data.buffer = new char[head.data_len];
		data.data_len = head.data_len;
		//先放变量，再放图片
		int offsetPrt = 0;
		for (int i = 0; i < inputPrams.getValueNums(); ++i)
		{
			memcpy(data.buffer + offsetPrt, inputPrams.m_Values[i].getDate(), sizeof(HValue));
			offsetPrt += sizeof(HValue);
		}
		for (int i = 0; i < inputImages.getImageNums(); ++i)
		{
			memcpy(data.buffer + offsetPrt, &inputImages.m_Images[i].imageHead, sizeof(ImageHeader));
			offsetPrt += sizeof(ImageHeader);
			memcpy(data.buffer + offsetPrt, inputImages.m_Images[i].data, inputImages.m_Images[i].imageHead.length);
			offsetPrt += inputImages.m_Images[i].imageHead.length;
		}
	}
	_SC_PacketData(char * inputbuffer)
	{
		memcpy(&head, inputbuffer, sizeof(SC_PackHead));
		data.buffer = new char[head.data_len];
		data.data_len = head.data_len;
		memcpy(data.buffer, inputbuffer + sizeof(SC_PackHead), head.data_len);
	}
	int getdatelength()
	{
		return sizeof(SC_PackHead) + head.data_len;
	}
	~_SC_PacketData()
	{
		if (data.buffer)
		{
			delete[] data.buffer;
			data.buffer = NULL;
		}
	}
	void * getHeaddata()
	{
		return &head;
	}
	void *GetDatadata()
	{
		return data.buffer;
	}
	void Clear()
	{
		head.init();
		data.init();
	}
}SC_PacketData;

typedef struct _SC_PacketDataEx
{
	//网络数据
	LONG DataSorceSocket = -1;//数据来源socket
	//通讯端口
	//包信息
	SC_PacketData data;
	_SC_PacketDataEx()
	{
		data.Clear();
	}
	~_SC_PacketDataEx()
	{
		data.Clear();
	}
	_SC_PacketDataEx(const _SC_PacketDataEx & aPacket)
	{
		this->DataSorceSocket = aPacket.DataSorceSocket;
		this->data = aPacket.data;
	}
	_SC_PacketDataEx(LONG inputComm, SC_PacketData inputData)
	{
		data = inputData;
	}
	_SC_PacketDataEx operator = (const _SC_PacketDataEx& aPacket)
	{
		this->DataSorceSocket = aPacket.DataSorceSocket;
		this->data = aPacket.data;
		return *this;
	}

}SC_PacketDataEx;