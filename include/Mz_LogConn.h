#pragma once
#include <tchar.h>

namespace Mz_Log
{
	class __declspec(dllexport) COperation
	{
		//初始化库
	public:
		int	Mz_Realse();
		int	Mz_GetInstance(const wchar_t* filename);
		int	Mz_SetLogFile(const wchar_t* pFileName);	//初始化日志文件
		int	Mz_AddLog(const wchar_t* _logStr);	//向日志文件中添加日志内容（字符数组）
        int   Mz_AddLogEx(const wchar_t* _logStr, const wchar_t* file, int line);
		int	Mz_CheckDateTime();				//日志一天一个  过一天则新写一个文件
	};
}
