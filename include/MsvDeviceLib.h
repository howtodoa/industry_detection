// MathLibrary.h - Contains declarations of math functions
#pragma once
#include <vector>
#include <string>

#include <opencv2/core.hpp>

#ifdef MZ_CAMERADEVICE_EXPORTS
#define MSV_STDC   __declspec(dllimport)
#else
#define MSV_STDC   __declspec(dllexport)
#endif

#if defined(__cplusplus)
#define MSV_EXTC extern "C"
#else
#define MSV_EXTC
#endif

typedef enum {
	DEVICE_TYPE_HIKVISION = 1,
	DEVICE_TYPE_DHUA = 2,
} DeviceType;

typedef enum {
	DEVICE_STATUS_DISCONNECTED = 0,
	DEVICE_STATUS_CONNECTED = 1,
	DEVICE_STATUS_ERROR = 2,
	DEVICE_STATUS_RECONNECTING = 3,
} DeviceStatus;

class class_net_engine;

#define  MZ_API MSV_EXTC int MSV_STDC
typedef void (*DeviceImageCallback)(cv::Mat& outputmat, void* pUser);
typedef struct DeviceId
{
	DeviceType m_type; //设备类型
	std::string m_id;    //64bit数据标示设备唯一标识码
	std::string m_ip;    //Ip
	std::string configFile; //配置文件
}MsvDeviceId;

namespace Mz_CameraConn
{
	//初始化库
	MZ_API MsvInitLib();
	//关闭库的使用
	MZ_API MsvCloseLib();
	//设备枚举
	MZ_API MsvEnumerateDevics(std::vector<MsvDeviceId>& deviceList);

	class __declspec(dllexport) COperation
	{
	public:
		COperation(MsvDeviceId inputID);
		~COperation();
		//打开设备
		int MsvOpenDevice();
		//关闭设备
		int MsvCloseDevice();
		//保存所有参数
		int MsvSaveParams(char* filename);
		int MsvLoadParams(char* filename);
		//
		int MsvStartImageCapture();
		//
		int MsvStopImageCapture();
		//设置触发模式
		//0 连续，1、单帧、2、多帧
		int MsvSetAcquisitionMode(int inputmode);
		int IsDevConnect();

		int SetParamEnum(const char* key, int Value);
		int GetParamEnum(const char* key, int* Value);

		int SetParam(char* key, bool Value);
		int GetParam(char* key, bool* Value);

		int SetParam(char* key, float Value);
		int GetParam(char* key, float* Value);

		int SetParam(char* key, int Value);
		int GetParam(char* key, int* Value);

		int SetParam(char* key, std::string Value);
		int GetParam(char* key, std::string* Value);

		int SetTrrigerModel(int model);
		int SetTrrigerSource(int source);
		int SoftTrigger();

		cv::Mat GetFrame(int Timeout);
		int RegisterImageCallBack(DeviceImageCallback inputCallbackfun, void* pUser);



	private:
		MsvDeviceId m_localDeviceID;
		class_net_engine* m_eng = nullptr;
	};
}
