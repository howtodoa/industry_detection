#pragma once
#include "PacketHead.h"
namespace Mz_ClientControl
{
	class __declspec(dllexport) ClientOperation
	{
	public:
		void InitSDK(CommPorts inputs);
		void FreeSDK();
		void StartWork();
		void StopWork();
		void DoActionFun(CommPorts portname, string funname, HValues inputValues, HImages inputImages, HValues* outPutValues, HImages* outPutImages, int* retCode, string* retMsg, int timeout = 1000);
		void RegsiterFunitFun(Callbackfunc func);
		void GetFunList_Remote(CommPorts portname,vector<Callbackfunc>& funList);
		void GetRemoteCommports(vector<CommPorts>& commports);
	};
}