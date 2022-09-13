#include "pch.h"
#include "Sqlite.h"

Sqlite::Sqlite() :hSqlite(nullptr)
{
}

Sqlite::~Sqlite()
{
	CloseDB();		// 关闭数据库连接
}

Sqlite& Sqlite::GetInstance()
{
	// 静态局部类对象
	static Sqlite instance;
	return instance;		// 返回的永远都是这个static地址
}

bool Sqlite::OpenDB(std::string path)
{
	// 连接数据库，如果不存在就会创建
	return (hSqlite || sqlite3_open(path.c_str(), &hSqlite) == SQLITE_OK);
}

void Sqlite::CloseDB()
{
	if (hSqlite && sqlite3_close(hSqlite) == SQLITE_OK)
	{
		hSqlite = nullptr;		// 数据库句柄重置
	}
}

bool Sqlite::IsConnected()const
{
	return hSqlite;		// 有句柄就会返回true
}

bool Sqlite::GetTables(std::vector<std::string>& lstTable)
{
	if (!IsConnected())
	{
		return FALSE;
	}

	/*因为要把表名放到vector里，单纯用Exec回调封装去查询会很复杂，直接调用stmt会更快*/
	sqlite3_stmt* pStmt;		// 不能做成类对象，因为可能多个函数同时操作这个指针
	sqlite3_prepare(hSqlite, "select name from sqlite_master where type='table' order by name", -1, &pStmt, 0);
	if (pStmt)
	{
		while (sqlite3_step(pStmt) == SQLITE_ROW)
		{
			char* name = (char*)sqlite3_column_text(pStmt, 0);
			lstTable.push_back(std::string(name));
		}
		sqlite3_finalize(pStmt);
		pStmt = nullptr;
		return TRUE;
	}
	return FALSE;
}

bool Sqlite::GetColumn(std::string strTable, std::string strColum)
{
	if (!IsConnected())
	{
		return FALSE;
	}

	bool result = FALSE;
	if (!Sqlite::GetInstance().Exec(Converter::combine_string("SELECT *FROM sqlite_master WHERE name = '", strTable, "' AND sql LIKE '%", strColum, "%'"), [](void* pThis, int argc, char** argv, char** col)->int {
		// 有返回结果就会进来回调，代表查询到对应的列名
		// 貌似一定要返回0，0应该是继续查找，不停止
		(*(bool*)pThis) = TRUE;		// 因为传进来的是void，强转成bool再取址就可以修改值
		return 0;
		}, &result))
	{
		return FALSE;
	}
		return result;
}

bool Sqlite::GetData(std::string tableName, std::vector<std::vector<std::string> >& lstData)
{
	if (!IsConnected())
	{
		return FALSE;
	}

	// 调用exec封装
	if (!Sqlite::GetInstance().Exec(Converter::combine_string("SELECT *FROM ", tableName).c_str(), [](void* pThis, int argc, char** argv, char** col)->int {
		// 有返回结果就会进来回调，代表查询到对应的列名
		// 貌似一定要返回0，0应该是继续查找，不停止
		for (int i = 0; i < argc; i++)
		{
			if (!i)
			{
				// i = 0的情况，就创建一个新vector，之后的数据就直接push_back到这个vector里就行
				// vector之间记得隔空格
				(*(std::vector<std::vector<std::string> >*)pThis).push_back(std::vector<std::string>(1, argv[i]));		// 初始化一个值。值为argv[0]
			}
			else if (i && argv[i] != NULL)		// 注意！！！这一块他妈调试了几个小时才发现，数据库返回值有可能返回NULL，所以要加NULL判断
			{
				// 把其他数据插入到vector后面
				(*(std::vector<std::vector<std::string> >*)pThis).back().push_back(argv[i]);
			}
		}
		return 0;
		}, &lstData))
	{
		return FALSE;
	}
		return TRUE;
}

std::string Sqlite::GetLastError()const
{
	// 获取错误信息Vector最后一个元素
	if (lstError.size())
	{
		return Converter::combine_string(lstError.back().first, "|", lstError.back().second);
	}
	return "No error have been finded";
}

bool Sqlite::Exec(std::string sql, int(*CallBack)(void*, int, char**, char**), void* pThis)
{
	if (!IsConnected())
	{
		return FALSE;
	}

	char* pError = nullptr;
	if (sqlite3_exec(hSqlite, sql.c_str(), (*CallBack), pThis, &pError) != SQLITE_OK)
	{
		// TODO: 改用Function类里的时间函数
		tm time = Time::get_date_time();

		lstError.push_back(make_pair(Converter::combine_string(
			time.tm_year + 1900, "年",
			time.tm_mon + 1, "月",
			time.tm_mday, "日 ",
			time.tm_hour, ":",
			time.tm_min, ":",
			time.tm_sec), std::string(pError)));
		return FALSE;
	}

	return TRUE;		// 成功返回true
}

bool Sqlite::Exec(std::string sql)
{
	if (!IsConnected())
	{
		return FALSE;
	}

	char* pError = nullptr;
	if (sqlite3_exec(hSqlite, sql.c_str(), NULL, NULL, &pError) != SQLITE_OK)
	{
		tm time = Time::get_date_time();


		lstError.push_back(make_pair(Converter::combine_string(
			time.tm_year + 1900, "年",
			time.tm_mon + 1, "月",
			time.tm_mday, "日 ",
			time.tm_hour, ":",
			time.tm_min, ":",
			time.tm_sec), std::string(pError)));
		return FALSE;
	}

	return TRUE;		// 成功返回true
}



