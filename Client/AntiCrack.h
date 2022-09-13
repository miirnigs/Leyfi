#pragma once
#include <string>
#include <objbase.h>
#include <random>
using namespace std;

class AntiCrack
{
public:
	/// <summary>
	/// 返回一个随机字符串
	/// (为了防止对random函数下断点，采用GUID或者C++11新增的random类生成随机字符串)
	/// </summary>
	/// <returns>生成的随机字符串</returns>
	string GetRandomString()
	{
		GUID guid;
		HRESULT hResult = CoCreateGuid(&guid);
		if (hResult == S_OK)
		{
			return ([](const GUID& guid)->string 
			{
				char buf[64] = { 0 };
				sprintf_s(buf, sizeof(buf),
					"%08X%04x%04x%02X%02x%02x%02x%02X%02x%02x%02x",
					guid.Data1, guid.Data2, guid.Data3,
					guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
					guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
				return string(buf);
				})(guid);
		}
		else
		{
			string buffer;		// buffer: 保存返回值
			std::default_random_engine e((unsigned int)time(nullptr));
			std::uniform_int_distribution<int> u(0, 35);		// 左闭右闭区间

			for (int i = 0; i < 32; i++) {
				char tmp = u(e);		// 随机一个小于 36 的整数，0-9、a-z 共 36 种字符
				if (tmp < 10) 
				{			
					// 如果随机数小于 10，变换成一个阿拉伯数字的 ASCII
					tmp += '0';
				}
				else
				{	
					// 否则，变换成一个小写字母的 ASCII
					tmp -= 10;
					tmp += 'a';
				}
				buffer += tmp;
			}
			return buffer;
		}
	}
};

