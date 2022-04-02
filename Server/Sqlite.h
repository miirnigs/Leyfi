#pragma once
#include "Functions.h"
#include <iostream>
#include <Windows.h>
#include <sqlite3.h>
#include <vector>
#include <utility>
using namespace functions;

 /*操作SQLite数据库*/
class Sqlite
{
	/*原本打算这个类做成基类的，想到只打开一个数据库，还是做成单例好一点*/
private:
	Sqlite();		// 私有默认构造函数，防止类外构造
	Sqlite(const Sqlite&) = delete;		// 删除拷贝函数
	Sqlite& operator=(const Sqlite&) = delete;
public:
	~Sqlite();
public:
	// 获取类实例，返回的局部对象引用，所以不用加锁
	static Sqlite& GetInstance();
public:
	// 打开数据库
	bool OpenDB(string path);
	// 关闭数据库
	void CloseDB();
	// 数据库是否已连接
	bool IsConnected()const;
	// 获取最后的错误信息
	string GetLastError()const;

	// 获取所有表(vector的引用，存放取得的表名)
	// PS: 无法获取错误信息
	bool GetTables(vector<string>& lstTable);
	// 判断对应的表内是否存在对应的列名
	bool GetColumn(string strTable, string strColum);

	// 从指定的表获取所有数据(要查询的表名，一个二维vector，用来存放取得的数据)
	// PS: vector与vector之间要隔一个空格，不然编译器会当成>>位运算符，vector里不能存char*，因为char*指向的地址在函数结束后就释放了，在外部是读取不到的，调试过了
	// 返回的数据用|分隔，用split可以分隔
	bool GetData(string tableName, vector<vector<string> >& lstData);

	// 执行SQL语句(sql语句 | 回调函数 | void指针[会被当成第一个参数传入回调函数])
	// PS: 这是对sqlite.exec函数的封装，本质上还是prepare->step->column->finalize
	bool Exec(string sql, int(*CallBack)(void*, int, char**, char**), void* pThis);
	// 执行SQL语句(sql语句) 没有回调函数，只会显示成功或者失败
	// PS:这是对sqlite.exec函数的封装，本质上还是prepare->step->column->finalize
	bool Exec(string sql);
private:
	sqlite3* hSqlite;		// 数据库句柄

	/*PS: 很坑！！里面的pair跟外面的vector要隔开一个空格！！不然编译器会识别成 >> 位操作符导致编译错误*/
	vector<pair<string, string> > lstError;		// 错误信息(时间 | 错误信息)

};