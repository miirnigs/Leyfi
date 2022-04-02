#pragma once
#include <iostream>
#include <Windows.h>
#include <time.h>
#include <sstream>
#include <string>
using namespace std;

// 一切需要的函数都在这
namespace functions
{
	/*时间类*/
	/*PS: 此类无需初始化*/
	class Time
	{
	public:
		Time() = delete;		// 初始化构造函数
		~Time() = default;

		// 获取当前时间，返回tm结构
		static tm GetDateTime();
	};
	
	/*字符串转换类*/
	/*PS: 此类无需初始化*/
	class Converter
	{
	public:
		Converter() = delete;;		// 初始化构造函数
		~Converter() = default;

		// 将所有传入的数据合并转成string
		template <typename T, typename... Arg>
		static string CombineString(const T& t, Arg&&... arg)		// 一个引用跟一个右值引用
		{
			stringstream ss;
			ss << t;
			return ss.str() + CombineString(forward<Arg>(arg)...);		// 完美转发
		}

		// 终止递归函数
		template <typename T>
		static string CombineString(const T& t)
		{
			stringstream ss;
			ss << t;
			return ss.str();
		}
	};

	/*路径类*/
	/*PS: 此类无需初始化*/
	class Path
	{
	public:
		Path() = delete;;		// 初始化构造函数
		~Path() = default;
		
		// 获取当前EXE路径，双斜杠结尾(默认不返回当前文件名)
		static string GetExePath(bool fileName = FALSE);
	};
}