#pragma once
// 要上预编译头的话，SQLite一定要用静态库的模式导入
// 如果用源码sqlite3.c的话，强行上预编译头会触发错误:
// error C2857: 在源文件中没有找到用 /Ycpch.h 命令行选项指定的“#include”语句

// 通用的头文件
#include "framework.h"
#include <string>
#include <iostream>
#include <Windows.h>
#include <time.h>
#include <sstream>
#include <io.h>

#include <vector>
#include <thread>
#include <mutex>
#include <WinSock2.h>		// 较新的头文件
#include <MSWSock.h>		// 一定要放在WinSock2.h的下面
#pragma comment(lib, "ws2_32.lib")
#include <functional>
#include <utility>

#include <WS2tcpip.h>

// 自定义的消息
#define WM_LIST_CHANGE		(WM_USER + 150)		// 自定义消息，告诉主界面卡密对话框卡密对话框的项被更改
#define WM_LIST_ADD			(WM_USER + 151)		// 自定义消息，往主界面的事件列表添加数据
#define WM_START_IOCP		(WM_USER + 152)		// 自定义消息，通知主界面开启IOCP服务器
#define WM_STOP_IOCP		(WM_USER + 153)		// 自定义消息，通知主界面关闭IOCP服务器

// 自定义标识符
#define TYPE_RUN	NULL	// IOCP的CompKey，指示线程继续运行
#define TYPE_CLOSE	1		// IOCP的CompKey，指示线程停止运行