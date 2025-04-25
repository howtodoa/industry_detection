#pragma once
#include "comutil.h"

namespace Mz_DB
{
	class __declspec(dllexport) DBOperation
	{
	public:
		int Mz_ConnectDB(wchar_t* connectstr);
		int Mz_CloseDB();
		int Mz_OpenRecord(wchar_t* connectstr);
		int Mz_CloseRecord();
		int Mz_GetRecordeValue(LPCTSTR strKey,int& Value);
		int Mz_SetRecordeValue(LPCTSTR strKey, int Value);

		int Mz_GetRecordeValue(LPCTSTR strKey, double& Value);
		int Mz_SetRecordeValue(LPCTSTR strKey, double Value);

		int Mz_GetRecordeValue(LPCTSTR strKey, float& Value);
		int Mz_SetRecordeValue(LPCTSTR strKey, float Value);

		int Mz_GetRecordeValue(LPCTSTR strKey, long& Value);
		int Mz_SetRecordeValue(LPCTSTR strKey, long Value);

		int Mz_GetRecordeValue(LPCTSTR strKey, _bstr_t& Value);
		int Mz_SetRecordeValue(LPCTSTR strKey, _bstr_t Value);

		int Mz_GetRecordeValue(LPCTSTR strKey, _SYSTEMTIME& Value);
		int Mz_SetRecordeValue(LPCTSTR strKey, _SYSTEMTIME Value);

		int Mz_UpdataRecorde();
		int Mz_AddNewRecorde();

	};
}