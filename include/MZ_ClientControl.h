#pragma once
#include "PacketHead.h"
namespace Mz_ClientControl
{
	class __declspec(dllexport) ClientOperation
	{
	public:
		void InitSDK(vector<CommPorts>inputs);
		void FreeSDK();
		void StartWork();
		void StopWork();
		void DoActionFun(string portname, string funname, HValues inputValues, HImages inputImages, HValues* outPutValues, HImages* outPutImages, int* retCode, string* retMsg);
	};
}