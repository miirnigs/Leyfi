#include "pch.h"
#include "Functions.h"

tm functions::Time::get_date_time()
{
	time_t now = time(0);
	tm types;		// 指向本地时间的tm结构体

	localtime_s(&types, &now);
	return types;
}

std::string functions::Path::get_exe_path(bool fileName)
{
	char szFilePath[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, szFilePath, MAX_PATH);

	if (!fileName)
	{
		// 不需要文件名
		(strrchr(szFilePath, '\\'))[1] = 0;
	}
	return std::string(szFilePath);
}
