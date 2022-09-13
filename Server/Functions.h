#pragma once

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
		static tm get_date_time();
	};
	
	/*字符串转换类*/
	/*PS: 此类无需初始化*/
	class Converter
	{
	public:
		Converter() = delete;;		// 初始化构造函数
		~Converter() = default;

		// 将所有传入的数据合并转成string
		template <class ...Args>
		static auto combine_string(Args ...args)->std::string
		{
			std::ostringstream os;
			int arr[] = { (([](std::ostringstream& os, auto t) {		// C++ 14加入了匿名函数模板化
				os << t;
				})(os,args),0)... };		// 匿名函数合并字符串，初始化列表展开参数包
			return os.str();
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
		static std::string get_exe_path(bool fileName = FALSE);
	};
}