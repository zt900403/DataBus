
//********************************************************************
//    DATABUS.H    文件注释
//    文件名  :    DATABUS.H
//    作者	 :    Tao Zhang
//    创建时间:    2014/10/25 23:46
//    文件描述:	用于和消息总线(DataBus)通信,在安装完Oracle Client端和
//				初始化Ado的前提下,可以访问数据库(Oracle10g)进行读取和写入操作.
//	Ado初始化:
//	MFC中CXXApp的Initinstance（）中		if(!AfxOleInit())
//										{
//											AfxMessageBox("AfxOleInit()初始化失败!");
//										}
//
//	Win32 Console App中					CoInitialize();
//										访问数据库代码
//										CoUninitialize();	
//	需要在程序根目录下创建一个配置文建,文件名为Config.ini,其内容如下:
//	
//	[Database]
//	ORACLE = Provider=OraOLEDB.Oracle.1;Data Source=ORCL;User ID=u1;Password=u1;
//	MaxItemNum = 120
//	ErrorLog = .\\myDBlog.txt
//	ImageItemName =	RWBS
//	ImagePathName = JGWJJDZ
//	[DataBus]
//	ErrorLog = .\\myGSOAPlog.txt
//	ReciMark = lq1

//*********************************************************************
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#ifdef DATA_BUS_API_EXPORTS
#define DATA_BUS_API _declspec(dllexport)
#else
#define DATA_BUS_API _declspec(dllimport)
#endif 

#include <string>
#include <vector>
#include <tchar.h>
#include <process.h>
#include <algorithm>


#define MY_DB_NEWITEM_MESSAGE	(WM_USER+1001)
#define TEMP_BUF_LEN 128
#define CONNECTION_TIMEOUT 5000																			//重连数据库超时时间 ,单位毫秒
#define GETDATA_HASNEW	1
#define GETDATA_TRUE	0
#define GETDATA_FALSE	-1


//********************************************************************
//    CMyDataBus    类注释
//    类名    :    CMyDataBus
//  基类名称:    NULL
//  命名空间:    NULL
//    作者    :    Tao Zhang
//    创建时间:    2014/10/25 23:56
//    类描述  :   用于和(DataBus)进行通信的类,分别实现了:
//				1.bool getTask()			获取单个未完成任务
//				2.bool getAllNewTask()		获取所有未完成任务
//				3.bool setTaskState()		设置任务状态
//				4.bool reportSystemInfo()	发送本机系统信息(心跳),确保程序运行正确
//*********************************************************************

//任务结构体
struct TaskInfo{
	std::string strTaskName;
	std::string strTaskState;
	std::string strTaskMark;
	std::string strTaskReciMark;
	__int64 jlsj;//lMsgID;
};
class DATA_BUS_API CMyDataBus
{
//构造函数
public:
	CMyDataBus();
	~CMyDataBus();

//内部辅助函数
private:
	std::string EncodeUtf8(std::wstring in);		// 宽字符转UTF8 ,处理字符乱码
	std::wstring DecodeUtf8(std::string in) ;		// UTF8 转宽字符 
	std::string ws2s(const std::wstring& ws);		//wstring转string
	std::wstring s2ws(const std::string& s);		//string 转wstring
	std::string getLocalTime();						//获得本机系统时间
	BOOL IsWinVersionEqualTo(DWORD dwMajorVersion, DWORD dwMinorVersion);		//比较系统版本,等于x.x版本
	BOOL IsWinVersionGreaterThan(DWORD dwMajorVersion, DWORD dwMinorVersion);	//比较系统版本,高于x.x版本
	BOOL IsWinVersionLessThan(DWORD dwMajorVersion, DWORD dwMinorVersion);		//比较系统版本,低于x.x版本
	char* getWindowsVersion(void);												//获得计算机版本
	__int64 CompareFileTime ( FILETIME time1, FILETIME time2 );					//比较系统时间,用于计算CPU使用率
	int getCpuUsage();															//获取CPU使用率
	bool writeErrorLog(char* czErrorContent,struct soap* soapClient=NULL);		//将连接结构体soap发生的错误写入错误日志,
//功能函数
public:
	bool getTask(int nTaskNumber);				//用于获得单个未完成任务,需要输入任务号
	bool getAllNewTask();						//用于获得所有未完成任务
	bool setTaskState(int nTaskNum,char* czTaskState);			//设置任务状态
	bool reportSystemInfo();					//发送本机系统信息(心跳)


//变量
public:


	struct TaskInfo		m_singleTaskInfo;		//保存获取单个任务详细信息的结构体
	std::vector<TaskInfo>	m_multiTaskInfo;	//保存获取多个任务详细信息的矢量
private:
	char m_czReciMark[TEMP_BUF_LEN];			//配置文件中,领取标识
	char m_czErrorLog[TEMP_BUF_LEN];			//配置文建中;错误日志地址
	FILE* m_fpErrorLog;							//错误日志文件指针
};



//********************************************************************
//    CMyDatabase    类注释
//    类名    :    CMyDatabase
//  基类名称:    
//  命名空间:    NULL
//    作者    :    Tao Zhang
//    创建时间:    2014/10/26 9:16
//    类描述  :     在安装完Oracle Client客户端和初始化Ado的前提下,用于与
//					数据库(Oracle10g)通信的类,分别实现了:
//					1.bool getData()		从数据库中获取数据
//					2,bool writeData()		将数据写入数据库
//*********************************************************************


//遥测数据结构体
struct RemoteDataUnit{
	CString	strVariableName;				//变量名
	int				nFlag;						//标记位,0表示没有数值,1表示有数值
	double			dValue;						//变量值
};
//typedef std::vector<RemoteDataUnit> v_RemoteData;		//外部获取数据矢量容器

class DATA_BUS_API CMyDatabase
{
//构造函数
public:
	CMyDatabase();
	~CMyDatabase();

//辅助函数
private:
	static std::string getLocalTime();										//获得本机时间
	static bool writeErrorLog(char* czErrorContent,_com_error *e = NULL);	//将错误写入错误日志
	static bool connectToDBUntilLinked();									//连接数据库直到连接成功(该函数会阻塞线程)
	static bool IsDisconnection();											//判断连接是否断开,如果断开返回true
	static BOOL getData(char* czTableName,char* czOrderBy="time");		//用于获取数据库数据,默认以数据库中time字段降序排列
	static bool getNewAddData(char*	czTableName,char* czOrderBy="time");	//获取数据库中新增数据
	static unsigned int __stdcall getDataThreadFun(LPVOID lpvoid);			//获取数据线程函数;
	static unsigned int __stdcall getNewItemThreadFun(LPVOID lpvoid);		//获取数据库中新增条目线程函数;

//功能函数
public:
	bool getRemoteData(RemoteDataUnit* __outAlterRemoteData,RemoteDataUnit* __outRemoteData=NULL);		//获取遥测数据,__outRemoteDataVector组合的所有遥测数据,__outAlterRemoteDataVecotr与上次调用变化的数据
	bool writeData(char* czTableName,int nID,char* czContent);		//用于将数据写入数据库
	CString getImageDirectory(char* czTableName,char* czIndex);	//获取图像处理结果文件夹
	CString getLastError();										//获得最后一次错误描述
	void setLastError(char* czErrorInfo);		//设置错误信息
	bool beginToScanDB(char* czTableName,HWND hSendToMesssage=NULL);		//开启扫描线程
	bool CloseScanDBThread();					//关闭扫描线程
//变量
public:

	
	static bool m_bDataThreadEndFlag;			//线程结束标志位,true为结束,false为进行中
	static HANDLE m_hScanDataThread;			//扫描数据线程句柄

private:
	
	char m_czImageItemName[TEMP_BUF_LEN];	//配置文件中;保存图像索引字段名
	char m_czImagePathName[TEMP_BUF_LEN];	//配置文件中;保存图像生成地址字段名
	
	static char m_czDBCon[TEMP_BUF_LEN];			//配置文件中;数据库连接字符串	
	static int m_nLinkTestCount;					//连接测试次数,只要一次出现"ORA-03114: 未连接到 ORALCE"错误代码便认定为连接断开
	
	static CString m_strLastError;			//记录最后一次错误描述
	static char m_czErrorLog[TEMP_BUF_LEN];		//配置文件中;错误日志地址
	static FILE* m_fpErrorLog;						//错误日志文件指针	
	static _ConnectionPtr	m_pDBConnection;		//数据库连接指针
	static char m_czMaxItemNum[TEMP_BUF_LEN];		//配置文件中;保存获取数据时,检索的最大数据条目个数
	static std::vector<RemoteDataUnit> m_RemoteData;//保存从数据库中获取的遥测数据的容器
	static std::vector<RemoteDataUnit> m_NewAddRemoteData;	//保存获取的遥测数据旧值
	static CRITICAL_SECTION m_g_cs;				//保证m_RemoteData的访问互斥
	static HANDLE m_hSemaphoreProducer;			//生产者信号量
	static HANDLE m_hSemaphoreConsumer;			//消费者信号量
	static CString m_strDBLastTime;			//数据库中最新条目的时间
	static HWND m_hSendToMessage;				//向该窗口发送数据库更新消息
	
};

