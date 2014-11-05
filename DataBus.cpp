
#include "stdafx.h"
#include "DataBus.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include ".\WebServices\WsTemplateServiceHttpBinding.nsmap"	
//////////////////////CMyDataBus [10/28/2014 Tao Zhang]/////////////////////////////////////
CMyDataBus::CMyDataBus()
{
	int ret = ::GetPrivateProfileStringA("DataBus","ErrorLog","",m_czErrorLog,TEMP_BUF_LEN,".\\Config.ini"); 
	if((ret == -1)||(ret == -2))
	{

		writeErrorLog("在Config.ini中,获取[DataBus]下的ErrorLog字段错误! ");
	}
	ret =  ::GetPrivateProfileStringA("DataBus","ReciMark","",m_czReciMark,TEMP_BUF_LEN,".\\Config.ini"); 
	if((ret == -1)||(ret == -2))
	{

		writeErrorLog("在Config.ini中,获取[DataBus]下的ReciMark字段错误! ");
	}

	m_fpErrorLog=NULL;

}
CMyDataBus::~CMyDataBus()
{
	if(m_fpErrorLog!=NULL)
	{
		fclose(m_fpErrorLog);
		m_fpErrorLog = NULL;
	}
}


// 宽字符转UTF8 ,处理字符乱码
std::string CMyDataBus::EncodeUtf8(std::wstring in)
{
	std::string s(in.length()*3+1,' '); 
	size_t len = ::WideCharToMultiByte(CP_UTF8, 0, 
		in.c_str(), in.length(), 
		&s[0], s.length(), 
		NULL, NULL); 
	s.resize(len); 
	return s; 
}


// UTF8 转宽字符 
std::wstring CMyDataBus::DecodeUtf8(std::string in) 
{ 
	std::wstring s(in.length(), ' '); //必需得添加"tchar.h"头文件
	size_t len = ::MultiByteToWideChar(CP_UTF8, 0, 
		in.c_str(), in.length(), 
		&s[0], s.length()); 
	s.resize(len); 
	return s ;
} 
//wstring转string
std::string CMyDataBus::ws2s(const std::wstring& ws)					
{
	std::string curLocale = setlocale(LC_ALL, NULL);        // curLocale = "C";
	setlocale(LC_ALL, "chs");
	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest,0,_Dsize);
	wcstombs(_Dest,_Source,_Dsize);
	std::string result = _Dest;
	delete []_Dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

//string 转wstring
std::wstring CMyDataBus::s2ws(const std::string& s)
{
	setlocale(LC_ALL, "chs"); 
	const char* _Source = s.c_str();
	size_t _Dsize = s.size() + 1;
	wchar_t *_Dest = new wchar_t[_Dsize];
	wmemset(_Dest, 0, _Dsize);
	mbstowcs(_Dest,_Source,_Dsize);
	std::wstring result = _Dest;
	delete []_Dest;
	setlocale(LC_ALL, "C");
	return result;
}

//获得本地时间
std::string CMyDataBus::getLocalTime()
{
	SYSTEMTIME stime;
	GetLocalTime(&stime);
	char str[100];
	sprintf(str,"%4d年%2d月%2d日%2d时%2d分",stime.wYear,stime.wMonth,stime.wDay,stime.wHour,stime.wMinute);
	std::string str1(str);
	return str1;
}

bool CMyDataBus::writeErrorLog(char* czErrorContent,struct soap* soapClient)
{
	if(soapClient==NULL)
	{
		m_fpErrorLog = fopen(m_czErrorLog,"a");
		if(m_fpErrorLog==NULL)
		{
			return false;
		}
		fputs(getLocalTime().c_str(),m_fpErrorLog);
		_fputts(_T(":"),m_fpErrorLog);
		fputs(czErrorContent,m_fpErrorLog);
		_fputts(_T("\n"),m_fpErrorLog);
		fflush(m_fpErrorLog);
		fclose(m_fpErrorLog);
	}
	else
	{
		m_fpErrorLog = fopen(m_czErrorLog,"a");
		if(m_fpErrorLog==NULL)
		{
			return false;
		}
		fputs(getLocalTime().c_str(),m_fpErrorLog);
		_fputts(_T(":"),m_fpErrorLog);
		fputs(czErrorContent,m_fpErrorLog);
		_fputts(_T("\n"),m_fpErrorLog);
		soap_print_fault(soapClient,m_fpErrorLog);
		_fputts(_T("\n"),m_fpErrorLog);
		fflush(m_fpErrorLog);
		fclose(m_fpErrorLog);
	}
	return true;
}

bool CMyDataBus::getTask(int nTaskNumber)
{
	struct soap soapClient;
	soap_init(&soapClient);
	soap_set_mode(&soapClient,SOAP_C_UTFSTRING);

	_ns1__ImageTask_USCOREGetItem	request;
	_ns1__ImageTask_USCOREGetItemResponse Response;

	char temp[TEMP_BUF_LEN];
	std::string str0 = "";
	str0+=_itoa(nTaskNumber,temp,10);
	std::string str1 = EncodeUtf8(s2ws(str0));
	request.in0 = &str1;
	soap_call___ns1__ImageTask_USCOREGetItem(&soapClient,NULL,"",&request,Response);
	if(soapClient.error)
	{
		writeErrorLog("bool CMyDataBus::getTask(int)获得单个任务失败,错误代码:",&soapClient);
		return false;
	}

	m_singleTaskInfo.jlsj=Response.out->jlsj[0];
	m_singleTaskInfo.strTaskName=ws2s(DecodeUtf8(Response.out->rwmc->c_str()));
	m_singleTaskInfo.strTaskMark=ws2s(DecodeUtf8(Response.out->rwbs->c_str()));
	m_singleTaskInfo.strTaskState=ws2s(DecodeUtf8(Response.out->rwclzt->c_str()));
	m_singleTaskInfo.strTaskReciMark=ws2s(DecodeUtf8(Response.out->rwlqxtbs->c_str()));

	soap_destroy(&soapClient);
	soap_end(&soapClient); 
	soap_done(&soapClient); 
	if(m_singleTaskInfo.jlsj==0)
	{
		return false;
	}
	else
		return true;
}

bool CMyDataBus::getAllNewTask()
{
	struct soap soapClient;
	soap_init(&soapClient);
	soap_set_mode(&soapClient,SOAP_C_UTFSTRING);

	_ns1__ImageTask_USCOREGetAllNewItem	request;
	_ns1__ImageTask_USCOREGetAllNewItemResponse Response;

	std::string str0 = m_czReciMark;
	std::string str1 = EncodeUtf8(s2ws(str0));
	request.in0 = &str1;
	soap_call___ns1__ImageTask_USCOREGetAllNewItem(&soapClient,NULL,"",&request,Response);
	if(soapClient.error)
	{
		writeErrorLog("bool CMyDataBus::getAllNewTask()获得多个任务失败,错误代码:",&soapClient);
		return false;
	}

	std::vector<TaskInfo>().swap(m_multiTaskInfo);			//清除多任务矢量中原有数据,并将他的容量最小
	m_multiTaskInfo.resize(Response.out->rs->RenWu.size());	//重新调整多任务矢量的大小

	for(int i=0;i<Response.out->rs->RenWu.size();i++)
	{
		m_multiTaskInfo[i].jlsj = Response.out->rs->RenWu[i]->jlsj[0];
		m_multiTaskInfo[i].strTaskMark=ws2s(DecodeUtf8(Response.out->rs->RenWu[i]->rwbs->c_str()));
		m_multiTaskInfo[i].strTaskName=ws2s(DecodeUtf8(Response.out->rs->RenWu[i]->rwmc->c_str()));
		m_multiTaskInfo[i].strTaskState=ws2s(DecodeUtf8(Response.out->rs->RenWu[i]->rwclzt->c_str()));
		m_multiTaskInfo[i].strTaskReciMark=ws2s(DecodeUtf8(Response.out->rs->RenWu[i]->rwlqxtbs->c_str()));
	}


	soap_destroy(&soapClient);
	soap_end(&soapClient); 
	soap_done(&soapClient); 
	if(m_multiTaskInfo.size()<=0)
	{
		return false;
	}
	else
		return true;

}

bool CMyDataBus::setTaskState(int nTaskNum,char* czTaskState)
{
	struct soap soapClient;
	soap_init(&soapClient);
	soap_set_mode(&soapClient,SOAP_C_UTFSTRING);

	_ns1__ImageTask_USCORESetItemState request;
	_ns1__ImageTask_USCORESetItemStateResponse	Response;

	char temp[TEMP_BUF_LEN];

	std::string str0 = EncodeUtf8(s2ws(_itoa(nTaskNum,temp,10)));
	std::string str1 = EncodeUtf8(s2ws(czTaskState));

	request.in0 = &str0;
	request.in1 = &str1;
	soap_call___ns1__ImageTask_USCORESetItemState(&soapClient,NULL,NULL,&request,Response);
	if(soapClient.error)
	{

		writeErrorLog("bool CMyDataBus::setTaskState(int,char*)设置任务状态失败,错误代码:",&soapClient);
		return false;
	}


	soap_destroy(&soapClient);
	soap_end(&soapClient); 
	soap_done(&soapClient); 
	return true;
}

//比较系统版本,等于x.x版本
BOOL CMyDataBus::IsWinVersionEqualTo(DWORD dwMajorVersion, DWORD dwMinorVersion)
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	//Initialize the OSVERSIONINFOEX structure.
	ZeroMemory(&osvi, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	osvi.dwMajorVersion = dwMajorVersion;
	osvi.dwMinorVersion = dwMinorVersion;

	//Initialize the condition mask.
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_EQUAL);

	return VerifyVersionInfo(
		&osvi,
		VER_MAJORVERSION | VER_MINORVERSION,
		dwlConditionMask
		);
}
//比较系统版本,高于x.x版本
BOOL CMyDataBus::IsWinVersionGreaterThan(DWORD dwMajorVersion, DWORD dwMinorVersion)
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	//Initialize the OSVERSIONINFOEX structure.
	ZeroMemory(&osvi, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	osvi.dwMajorVersion = dwMajorVersion;
	osvi.dwMinorVersion = dwMinorVersion;

	//system major version > dwMajorVersion
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER);
	if (VerifyVersionInfo(&osvi, VER_MAJORVERSION, dwlConditionMask))
		return TRUE;

	//sytem major version = dwMajorVersion && minor version > dwMinorVersion
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_GREATER);

	return VerifyVersionInfo(
		&osvi,
		VER_MAJORVERSION | VER_MINORVERSION,
		dwlConditionMask
		);
}
//比较系统版本,低于x.x版本
BOOL CMyDataBus::IsWinVersionLessThan(DWORD dwMajorVersion, DWORD dwMinorVersion)
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	//Initialize the OSVERSIONINFOEX structure.
	ZeroMemory(&osvi, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	osvi.dwMajorVersion = dwMajorVersion;
	osvi.dwMinorVersion = dwMinorVersion;

	//system major version < dwMajorVersion
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_LESS);
	if (VerifyVersionInfo(&osvi, VER_MAJORVERSION, dwlConditionMask))
		return TRUE;

	//sytem major version = dwMajorVersion && minor version < dwMinorVersion
	dwlConditionMask = 0;
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, VER_LESS);

	return VerifyVersionInfo(
		&osvi,
		VER_MAJORVERSION | VER_MINORVERSION,
		dwlConditionMask
		);
}
//获得计算机版本
char* CMyDataBus::getWindowsVersion(void)
{
	/* Ancient Windows */
	if (IsWinVersionLessThan(5, 1)) {
		return "WINDOWS_ANCIENT(Less than 5.1(XP)";
	}
	/* Windows XP */
	else if (IsWinVersionEqualTo(5, 1)) {
		return "WINDOWS_XP";
	}
	/* Windows Server 2003 */
	else if (IsWinVersionEqualTo(5, 2)) {
		return "WINDOWS_SERVER_2003";
	}
	/* Windows Vista, Windows Server 2008 */
	else if (IsWinVersionEqualTo(6, 0)) {
		return "WINDOWS_VISTA";
	}
	/* Windows 7, Windows Server 2008 R2 */
	else if (IsWinVersionEqualTo(6, 1)) {
		return "WINDOWS_7";
	}
	/* Windows 8 */
	else if (IsWinVersionEqualTo(6, 2)) {
		return "WINDOWS 8";
	}
	else if (IsWinVersionGreaterThan(6,2)) {
		//WindowsVersion = WINDOWS_NEW;
		return "WINDOWS_NEW(Greater than 6.2(win8))";
	}
	else
		return "CAN_NOT_GET_WINDOWS_VERSION";
}


//比较系统时间,用于计算CPU使用率
__int64 CMyDataBus::CompareFileTime( FILETIME time1, FILETIME time2 )
{
	__int64 a = time1.dwHighDateTime << 32 | time1.dwLowDateTime ;
	__int64 b = time2.dwHighDateTime << 32 | time2.dwLowDateTime ;

	return   (b - a);
}
//获取CPU使用率
int CMyDataBus::getCpuUsage()
{
	HANDLE hEvent;
	BOOL res ;

	static FILETIME preidleTime;
	static FILETIME prekernelTime;
	static FILETIME preuserTime;

	static FILETIME idleTime;
	static FILETIME kernelTime;
	static FILETIME userTime;

	res = GetSystemTimes( &idleTime, &kernelTime, &userTime );

	preidleTime = idleTime;
	prekernelTime = kernelTime;
	preuserTime = userTime ;

	hEvent = CreateEvent (NULL,FALSE,FALSE,NULL); // 初始值为 nonsignaled ，并且每次触发后自动设置为nonsignaled


	WaitForSingleObject( hEvent,1000 ); //等待500毫秒
	res = GetSystemTimes( &idleTime, &kernelTime, &userTime );

	int idle = CompareFileTime( preidleTime,idleTime);
	int kernel = CompareFileTime( prekernelTime, kernelTime);
	int user = CompareFileTime(preuserTime, userTime);

	int cpu = (kernel +user - idle) *100/(kernel+user);
	int cpuidle = ( idle) *100/(kernel+user);



	preidleTime = idleTime;
	prekernelTime = kernelTime;
	preuserTime = userTime ;
	return cpu;
}

bool CMyDataBus::reportSystemInfo()
{
	struct soap soapClient;
	soap_init(&soapClient);
	soap_set_mode(&soapClient,SOAP_C_UTFSTRING);

	_ns1__ReportSystemInfo request;
	_ns1__ReportSystemInfoResponse	Response;

	//获取系统版本
	std::string str0 = getWindowsVersion();

	std::string str1 = "1";

	//获取计算机名称
	WCHAR szTemp[MAX_PATH];
	DWORD len=sizeof(szTemp); 
	GetComputerName(szTemp,&len);
	std::wstring str2 = szTemp;
	std::string str6 = EncodeUtf8(str2);

	//获得本机IP地址
	char buf[TEMP_BUF_LEN]="";
	struct hostent *ph = 0;
	WSADATA wsaData;
	int ret=WSAStartup(MAKEWORD(2,2),&wsaData);
	if (ret!=0)
	{
		writeErrorLog("bool CMyDataBus::reportSystemInfo()发送系统信息失败,初始化WSAStartup()失败(版本为SOCKET2.2)");
		return false;
	}
	gethostname(buf, TEMP_BUF_LEN);
	ph = gethostbyname(buf);
	const char *IP =inet_ntoa(*((struct in_addr *)ph->h_addr_list[0]));//此处获得本机IP
	WSACleanup();
	std::string str3 = IP;

	//获得CPU使用率
	char temp[TEMP_BUF_LEN];
	std::string str4 = _itoa(getCpuUsage(),temp,10);
	str4 += "%";
	//获得内存使用率
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof (statex);
	GlobalMemoryStatusEx (&statex);
	std::string str5 = _itoa(statex.dwMemoryLoad,temp,10);
	str5 += "%";

	request.in0 = &str0;
	request.in1 = &str1;
	request.in2 = &str6;
	request.in3 = &str3;
	request.in4 = &str4;
	request.in5 = &str5;

	soap_call___ns1__ReportSystemInfo(&soapClient,NULL,NULL,&request,Response);
	if(soapClient.error)
	{

		writeErrorLog("bool CMyDataBus::reportSystemInfo()发送系统信息失败,错误代码:",&soapClient);
		return false;
	}


	soap_destroy(&soapClient);
	soap_end(&soapClient); 
	soap_done(&soapClient); 
	return true;
}

//////////////////////CMyDataBus [10/28/2014 Tao Zhang]/////////////////////////////////////


//////////////////////CMyDatabase [10/28/2014 Tao Zhang]/////////////////////////////////////



CMyDatabase::CMyDatabase()
{


	int ret = ::GetPrivateProfileStringA("Database","ORACLE","",m_czDBCon,TEMP_BUF_LEN,".\\Config.ini"); 
	if((ret == -1)||(ret == -2))
	{

		writeErrorLog("在Config.ini中,获取[Datebase]下的ORACLE字段错误! ");
	}
	ret = ::GetPrivateProfileStringA("Database","MaxItemNum","",m_czMaxItemNum,TEMP_BUF_LEN,".\\Config.ini");
	if((ret == -1)||(ret == -2))
	{

		writeErrorLog("在Config.ini中,获取[Database]下的MaxItemNum字段错误! ");
	}
	ret = ::GetPrivateProfileStringA("Database","ErrorLog","",m_czErrorLog,TEMP_BUF_LEN,".\\Config.ini");
	if((ret == -1)||(ret == -2))
	{

		writeErrorLog("在Config.ini中,获取[Database]下的ErrorLog字段错误!  ");
	}
	ret = ::GetPrivateProfileStringA("Database","ImageItemName","",m_czImageItemName,TEMP_BUF_LEN,".\\Config.ini");
	if((ret == -1)||(ret == -2))
	{

		writeErrorLog("在Config.ini中,获取[Database]下的ImageItemName字段错误!  ");
	}
	ret = ::GetPrivateProfileStringA("Database","ImagePathName","",m_czImagePathName,TEMP_BUF_LEN,".\\Config.ini");
	if((ret == -1)||(ret == -2))
	{

		writeErrorLog("在Config.ini中,获取[Database]下的ImagePathName字段错误! ");
	}

	if(m_pDBConnection==NULL)
		if (FAILED(m_pDBConnection.CreateInstance(__uuidof(Connection))))
		{
			writeErrorLog("bool CMyDatabase::writeData(char* TableName,int nID,char* Content)函数中数据库链接实例创建失败！");	
		}

		if(m_pDBConnection->GetState()==adStateClosed)
		{
			try
			{
				m_pDBConnection->Open((_bstr_t)m_czDBCon,"","",NULL);
			}
			catch(_com_error &e)
			{
				writeErrorLog("CMyDatabase::CMyDatabase()构造函数中,第一次连接数据库失败!",&e);
			}
		}
		while(m_pDBConnection->GetState()==adStateClosed)
		{
			try
			{
				m_pDBConnection->Open((_bstr_t)m_czDBCon,"","",NULL);
			}
			catch(_com_error &e)
			{
				writeErrorLog("CMyDatabase::CMyDatabase()构造函数中,while()连接数据库失败!",&e);
			}
			Sleep(CONNECTION_TIMEOUT);
		}

		m_nLinkTestCount = 5;					//测试连接5次,如果5次不成功判断为断开连接
		m_fpErrorLog=NULL;

		InitializeCriticalSectionAndSpinCount(&m_g_cs,4000);

}
CMyDatabase::~CMyDatabase()
{
	if(m_fpErrorLog!=NULL)
	{
		fclose(m_fpErrorLog);
		m_fpErrorLog =NULL;
	}

	if(m_hScanDataThread!=NULL)
	{
		m_bDataThreadEndFlag = true;
		ReleaseSemaphore(m_hSemaphoreProducer,1,NULL);
		WaitForSingleObject(m_hScanDataThread,INFINITE);
		CloseHandle(m_hScanDataThread);
		DeleteCriticalSection(&m_g_cs);
		CloseHandle(m_hSemaphoreProducer);
		CloseHandle(m_hSemaphoreConsumer);
		m_hScanDataThread=NULL;
		m_hSemaphoreProducer=NULL;
		m_hSemaphoreConsumer=NULL;
	}

	if(m_pDBConnection->GetState()==adStateOpen)
	{
		m_pDBConnection->Close();
	}
	if (m_pDBConnection)
	{
		m_pDBConnection.Release();
		m_pDBConnection = NULL;
	}

	
	DeleteCriticalSection(&m_g_cs);

}


//扫描数据线程函数
unsigned int __stdcall CMyDatabase::getDataThreadFun(LPVOID lpvoid)
{

	while(m_bDataThreadEndFlag!=true)
	{

		
		EnterCriticalSection(&m_g_cs);
		int nReturnValue=getData((char*)lpvoid);
		LeaveCriticalSection(&m_g_cs);
		
		if(nReturnValue==GETDATA_HASNEW)
		{
			static int nNewAddNum = m_NewAddRemoteData.size();
			WaitForSingleObject(m_hSemaphoreProducer,INFINITE);
			PostMessage(m_hSendToMessage,MY_DB_NEWITEM_MESSAGE,(WPARAM)&nNewAddNum,NULL);
			ReleaseSemaphore(m_hSemaphoreConsumer,1,NULL);
		}
		Sleep(10);
	}
	return 123;
}

//开启扫描线程
bool CMyDatabase::beginToScanDB(char* czTableName,HWND hSendToMessage)
{
	m_bDataThreadEndFlag=false;
	m_hSendToMessage=hSendToMessage;
	if(m_hScanDataThread==NULL)
	{
		m_hScanDataThread=(HANDLE)_beginthreadex(NULL,0,getDataThreadFun,czTableName,0,NULL);

		m_hSemaphoreProducer=CreateSemaphore(NULL, 1, 1, NULL);
		m_hSemaphoreConsumer=CreateSemaphore(NULL, 0, 1, NULL);
	}
	return true;
}
//设置错误信息
void CMyDatabase::setLastError(char* czErrorInfo)
{
	m_strLastError="";
	m_strLastError=czErrorInfo;
	
}

//关闭扫描线程
bool CMyDatabase::CloseScanDBThread()
{
	if(m_hScanDataThread!=NULL)
	{	

		m_bDataThreadEndFlag = true;
		ReleaseSemaphore(m_hSemaphoreProducer,1,NULL);
		WaitForSingleObject(m_hScanDataThread,INFINITE);
		CloseHandle(m_hScanDataThread);
		CloseHandle(m_hSemaphoreProducer);
		CloseHandle(m_hSemaphoreConsumer);
		m_hScanDataThread=NULL;
	}

	return true;
}

bool CMyDatabase::getRemoteData(RemoteDataUnit* __outAlterRemoteData,RemoteDataUnit* __outRemoteData)
{

	if(m_hScanDataThread!=NULL)
	{
		WaitForSingleObject(m_hSemaphoreConsumer,INFINITE);
		EnterCriticalSection(&m_g_cs);
		if(__outRemoteData!=NULL)
		{
			for(int i=0;i<m_RemoteData.size();i++)
			{
				__outRemoteData[i].strVariableName	= m_RemoteData[i].strVariableName;
				__outRemoteData[i].dValue	=	m_RemoteData[i].dValue;
				__outRemoteData[i].nFlag	=	m_RemoteData[i].nFlag;
				if(__outRemoteData[i].nFlag==0)
				{
					char temp[50];
					CString str(__outRemoteData[i].strVariableName);
					str += "遥测数据获取失败!,原因可能是搜索数据库中行数太少,当前为";
					str += m_czMaxItemNum;
					str += "行,请增加Config.ini中MaxItemNum的值.";
					m_strLastError = str;
					return false;
				}
			}
		}

	
		for(int i=0;i<m_NewAddRemoteData.size();i++)
		{
			__outAlterRemoteData[i].strVariableName=m_NewAddRemoteData[i].strVariableName;
			__outAlterRemoteData[i].nFlag=m_NewAddRemoteData[i].nFlag;
			__outAlterRemoteData[i].dValue=m_NewAddRemoteData[i].dValue;
		}		
		LeaveCriticalSection(&m_g_cs);
		ReleaseSemaphore(m_hSemaphoreProducer,1,NULL);	
		return true;
	}
	else
	{
		setLastError("扫描数据库线程未开启!");
		return false;
	}
}

//返回最后一次错误描述
CString CMyDatabase::getLastError()
{
	return m_strLastError;
}

//连接数据库直到连接成功(该函数会阻塞线程)
bool CMyDatabase::connectToDBUntilLinked()
{
	if(m_pDBConnection->GetState()==adStateOpen)
	{
		m_pDBConnection->Close();
	}
	while(m_pDBConnection->GetState()==adStateClosed)
	{
		try
		{
			m_pDBConnection->Open((_bstr_t)m_czDBCon,"","",NULL);
		}
		catch(_com_error &e)
		{

			writeErrorLog("bool CMyDatabase::connectToDBUntilLinked()中连接失败!错误描述:",&e);
		}
		Sleep(CONNECTION_TIMEOUT);
	}
	return true;
}

//判断连接是否断开,如果断开返回true
bool CMyDatabase::IsDisconnection()
{
	while(m_nLinkTestCount--)
	{
		try
		{
			m_pDBConnection->Execute((_bstr_t)"show user",NULL,adCmdText);
		}
		catch (_com_error& e)
		{
			writeErrorLog("bool CMyDatabase::IsDisconnection()中执行检测连接语句失败!错误描述:",&e);
			if(strcmp("ORA-03114: 未连接到 ORALCE",e.Description())==0)
			{
				return true;
			}
		}
	}
	m_nLinkTestCount=5;									//恢复检测初值
	return false;
}

//获得本地时间
std::string CMyDatabase::getLocalTime()
{
	SYSTEMTIME stime;
	GetLocalTime(&stime);
	char str[100];
	sprintf(str,"%4d年%2d月%2d日%2d时%2d分",stime.wYear,stime.wMonth,stime.wDay,stime.wHour,stime.wMinute);
	std::string str1(str);
	return str1;
}


//将错误写入错误日志
bool CMyDatabase::writeErrorLog(char* czErrorContent,_com_error *e)
{
	if(e==NULL)
	{

		m_fpErrorLog = fopen(m_czErrorLog,"a");
		if(m_fpErrorLog==NULL)
		{
			return false;
		}
		fputs(getLocalTime().c_str(),m_fpErrorLog);
		_fputts(_T(":"),m_fpErrorLog);
		fputs(czErrorContent,m_fpErrorLog);
		fputs("\n",m_fpErrorLog);
		fflush(m_fpErrorLog);
		fclose(m_fpErrorLog);
		m_strLastError=czErrorContent;
	}
	else
	{

		m_fpErrorLog = fopen(m_czErrorLog,"a");
		if(m_fpErrorLog==NULL)
		{
			return false;
		}
		fputs(getLocalTime().c_str(),m_fpErrorLog);
		_fputts(_T(":"),m_fpErrorLog);
		fputs(czErrorContent,m_fpErrorLog);
		fputs(e->Description(),m_fpErrorLog);
		_fputts(_T("\n"),m_fpErrorLog);
		fflush(m_fpErrorLog);
		fclose(m_fpErrorLog);
		m_strLastError=czErrorContent;
		m_strLastError+=(char*)(e->Description());

	}
	return true;
}


//用于获取数据库数据,默认以数据库中time字段降序排列.
BOOL CMyDatabase::getData(char* czTableName,char* czOrderBy)
{

	if(m_pDBConnection->GetState()==adStateOpen)
	{
		int nReturnValue=0;
		_RecordsetPtr pMyRecordset = NULL;

		if(FAILED(pMyRecordset.CreateInstance(__uuidof(Recordset))))
		{
			writeErrorLog("bool CMyDatabase::getData(char* TableName,char* OrderBy)函数中数据库记录集实例创建失败！");
			return GETDATA_FALSE;
		}

		std::string sql = "select * from ( select * from ";
		sql += czTableName;
		sql += " order by ";
		sql += czOrderBy;
		sql += " desc )  where rownum<=";
		sql += m_czMaxItemNum;

		try
		{
			pMyRecordset->Open((_variant_t)sql.c_str()
				,m_pDBConnection.GetInterfacePtr()
				,adOpenStatic
				,adLockOptimistic
				,adCmdText);
		}
		catch(_com_error &e)
		{
			writeErrorLog("bool CMyDatabase::getData(char* TableName,char* OrderBy)函数中sql语句执行失败(select * from ( select * from TableName order by OrderBy  desc )  where rownum<m_czMaxItemNum),错误代码:",&e);
			if(IsDisconnection())
			{
				connectToDBUntilLinked();
				int nReturnValueTemp;
				nReturnValueTemp=getData(czTableName,czOrderBy);
				if(nReturnValueTemp==GETDATA_TRUE)
				{
					return GETDATA_TRUE;
				}
				else if(nReturnValueTemp==GETDATA_HASNEW)
				{
					return GETDATA_HASNEW;
				}
				else
				{
					return GETDATA_FALSE;
				}

			}
			return GETDATA_FALSE;
		}
		int nColumnNum = pMyRecordset->Fields->Count-1;
		try
		{

			if(m_RemoteData.size()!=nColumnNum)
			{
				std::vector<RemoteDataUnit>().swap(m_RemoteData);
				m_RemoteData.resize(nColumnNum);
			}
			for(int i=0;i<nColumnNum;i++)
			{
				m_RemoteData[i].nFlag=0;
			}
			if(nColumnNum>3)
			{
				std::string strTemp1= pMyRecordset->Fields->GetItem((_variant_t)((long)nColumnNum))->GetName();
				std::string strTemp2=pMyRecordset->Fields->GetItem((_variant_t)((long)nColumnNum-1))->GetName();
				std::string strTemp3=pMyRecordset->Fields->GetItem((_variant_t)((long)nColumnNum-2))->GetName();
				if((m_RemoteData[nColumnNum-1].strVariableName!=strTemp1.c_str())||(m_RemoteData[nColumnNum-2].strVariableName!=strTemp2.c_str())||(m_RemoteData[nColumnNum-3].strVariableName!=strTemp3.c_str()))
				{
					for(int i=0;i<nColumnNum;i++)
					{
						m_RemoteData[i].strVariableName=(char*)(pMyRecordset->Fields->GetItem((_variant_t)((long)i+1))->GetName());
					}
				}
			}
			else
			{
				for(int i=0;i<nColumnNum;i++)
				{
					m_RemoteData[i].strVariableName=(char*)(pMyRecordset->Fields->GetItem((_variant_t)((long)i+1))->GetName());
				}
			}
		}
		catch(_com_error& e)
		{
			writeErrorLog("bool CMyDatabase::getData(char* TableName,char* OrderBy)函数中获取数据列数失败!获取数据列名失败!");
			return GETDATA_FALSE;
		}

		try
		{

			if(!pMyRecordset->BOF)
			{

				pMyRecordset->MoveFirst();
				while(!pMyRecordset->adoEOF)
				{

					int nReadAllDataFlag=1;
					for(int i=0;i<nColumnNum;i++)
					{
						_variant_t TheValue;
						if(m_RemoteData[i].nFlag==0)
						{
							TheValue = pMyRecordset->GetCollect(m_RemoteData[i].strVariableName.GetBuffer()); 

							if(TheValue.vt!=VT_NULL)
							{
								m_RemoteData[i].dValue = (double)TheValue;
								m_RemoteData[i].nFlag	=	1;
							}
							nReadAllDataFlag &= m_RemoteData[i].nFlag;
						}
					}
					if(nReadAllDataFlag==1)
						break;
					pMyRecordset->MoveNext();
				}
			}
			else
			{
				writeErrorLog("bool CMyDatabase::getData(char* TableName,char* OrderBy)函数中所打开的记录集为空集(可能表中没有数据)");
				return GETDATA_FALSE;
			}

		}
		catch(_com_error &e)
		{
			writeErrorLog("bool CMyDatabase::getData(char* TableName,char* OrderBy)函数中遍历获取遥测数据失败,错误代码:",&e);
			return GETDATA_FALSE;
		}

		if(pMyRecordset->GetState()==adStateOpen)
		{
			pMyRecordset->Close();
			pMyRecordset=NULL;
		}
		if(FAILED(pMyRecordset.CreateInstance(__uuidof(Recordset))))
		{
			writeErrorLog("bool CMyDatabase::getData(char* TableName,char* OrderBy)函数中数据库记录集实例创建失败！");
			return GETDATA_FALSE;
		}
		try
		{/*
			CString sql2("select to_char(");
			sql2 +=czOrderBy;

			sql2 +=",'yyyy-mm-dd hh24:mi:ss') from ";
			sql2 += czTableName;
			sql2 +=" order by ";
			sql2 += czOrderBy;
			sql2	+= " desc";*/
			CString sql2("select * from ");
			sql2 +=czTableName;
			sql2 +=" order by ";
			sql2 += czOrderBy;
			sql2 += " desc";
			pMyRecordset->Open((_variant_t)sql2.GetBuffer()
				,m_pDBConnection.GetInterfacePtr()
				,adOpenStatic
				,adLockOptimistic
				,adCmdText);
			

			if(!pMyRecordset->BOF)
			{
				pMyRecordset->MoveFirst();
				_variant_t TheTime = pMyRecordset->Fields->GetItem((_variant_t)((long)0))->Value;
				COleDateTime t(TheTime.date);
				CString strDate=t.Format();

				if(m_strDBLastTime.Compare(strDate)==0)
				{
					nReturnValue = GETDATA_TRUE;
				}
				else
				{
					m_NewAddRemoteData.resize(0);
					for(int i=0;i<nColumnNum;i++)
					{
						_variant_t TheValue;
						TheValue = pMyRecordset->GetCollect(m_RemoteData[i].strVariableName.GetBuffer()); 
						if(TheValue.vt!=VT_NULL)
						{
							RemoteDataUnit  tempRDU;
							tempRDU.strVariableName=m_RemoteData[i].strVariableName;
							tempRDU.dValue=(double)TheValue;
							tempRDU.nFlag=1;
							m_NewAddRemoteData.push_back(tempRDU);
						}
					}
					m_strDBLastTime=strDate;
					nReturnValue = GETDATA_HASNEW;
				}
			}
		}
		catch(_com_error &e)
		{
			writeErrorLog("bool CMyDatabase::getData(char* TableName,char* OrderBy)函数中获取时间to_char失败,错误代码:",&e);
			return GETDATA_FALSE;
		}
		if(pMyRecordset->GetState()==adStateOpen)
		{
			pMyRecordset->Close();
			pMyRecordset=NULL;
		}
		return nReturnValue;
	}
	else
		return GETDATA_FALSE;
}

bool CMyDatabase::writeData(char* czTableName,int nID,char* czContent)
{
	if(m_pDBConnection->GetState()==adStateOpen)
	{
		std::string sql = "INSERT INTO ";
		sql += czTableName;
		sql += " VALUES(";
		char temp[50];
		sql += _itoa_s(nID,temp,10);
		sql += ",'";
		sql += czContent;
		sql += "')";

		try
		{
			m_pDBConnection->Execute((_bstr_t)sql.data(),NULL,adCmdText);
		}
		catch(_com_error &e)
		{
			writeErrorLog("bool CMyDatabase::writeData(char* TableName,int nID,char* Content)函数中数据库连接失败,错误描述:",&e);
			if(IsDisconnection())
			{
				connectToDBUntilLinked();
				if(writeData(czTableName,nID,czContent))
				{
					return true;
				}
			}
			return false;
		}

		return true;
	}
	else
		return false;
}

CString CMyDatabase::getImageDirectory(char* czTableName,char* czIndex)
{
	CString strResult("");
	if(m_pDBConnection->GetState()==adStateOpen)
	{
		_RecordsetPtr pMyRecordset = NULL;
		if(FAILED(pMyRecordset.CreateInstance(__uuidof(Recordset))))
		{
			writeErrorLog("std::string CMyDatabase::getImageDirectory(char*,char*)函数中记录集实例创建失败！");
			strResult = "Error";
			return strResult;
		}


		std::string sql = "SELECT * FROM ";
		sql += czTableName;
		sql += " WHERE ";
		sql += m_czImageItemName;
		sql += "='";
		sql += czIndex;
		sql += "'";

		try
		{
			pMyRecordset->Open((_variant_t)sql.c_str()
				,m_pDBConnection.GetInterfacePtr()
				,adOpenStatic
				,adLockOptimistic
				,adCmdText);
		}
		catch(_com_error &e)
		{

			writeErrorLog("std::string CMyDatabase::getImageDirectory(char*,char*)函数中sql语句执行失败(SELECT * FROM TableName	WHERE m_czImageItemName='czIndex')！错误代码:",&e);
			if(IsDisconnection())
			{
				connectToDBUntilLinked();
				strResult=getImageDirectory(czTableName,czIndex);
				if(strResult!="Error")
				{
					if (pMyRecordset->GetState()==adStateOpen)
					{
						pMyRecordset->Close();
						pMyRecordset=NULL;
					}
					return strResult;
				}
			}
			strResult ="Error";
			if (pMyRecordset->GetState()==adStateOpen)
			{
				pMyRecordset->Close();
				pMyRecordset=NULL;
			}
			return strResult;
		}


		_variant_t TheValue;
		try
		{

			if(!pMyRecordset->BOF)
			{
				pMyRecordset->MoveFirst();
				while(!pMyRecordset->adoEOF)
				{
					TheValue = pMyRecordset->GetCollect(m_czImagePathName);
					if(TheValue.vt!=VT_NULL)
					{
						break;
					}
					pMyRecordset->MoveNext();
				}

			}
			else
			{	
				writeErrorLog("bool CMyDatabase::getImageDirectory(char* ,char* OrderBy)函数中所打开的记录集为空集(可能表中没有数据)");
				strResult ="Error";
				if (pMyRecordset->GetState()==adStateOpen)
				{
					pMyRecordset->Close();
					pMyRecordset=NULL;
				}
				return strResult;
			}

		}
		catch(_com_error &e)
		{
			writeErrorLog("bool CMyDatabase::getImageDirectory(char* TableName,char*)获取图像文件地址信息失败!,错误代码:",&e);
			strResult ="Error";
			if (pMyRecordset->GetState()==adStateOpen)
			{
				pMyRecordset->Close();
				pMyRecordset=NULL;
			}
			return strResult;
		}


		strResult=((char*)(_bstr_t)TheValue);
		if (pMyRecordset->GetState()==adStateOpen)
		{
			pMyRecordset->Close();
			pMyRecordset=NULL;
		}
		return strResult;
	}
	else
	{	
		strResult ="Error";
		return strResult;
	}
} 
//////////////////////CMyDatabase [10/28/2014 Tao Zhang]/////////////////////////////////////

HANDLE CMyDatabase::m_hScanDataThread=NULL;
HANDLE CMyDatabase::m_hSemaphoreProducer=NULL;
HANDLE CMyDatabase::m_hSemaphoreConsumer=NULL;
HWND CMyDatabase::m_hSendToMessage=NULL;	
CRITICAL_SECTION CMyDatabase::m_g_cs;	


char CMyDatabase::m_czDBCon[TEMP_BUF_LEN];
int CMyDatabase::m_nLinkTestCount=5;
bool CMyDatabase::m_bDataThreadEndFlag;
char CMyDatabase::m_czErrorLog[TEMP_BUF_LEN];
FILE* CMyDatabase::m_fpErrorLog;
_ConnectionPtr	CMyDatabase::m_pDBConnection=NULL;
char CMyDatabase::m_czMaxItemNum[TEMP_BUF_LEN];
std::vector<RemoteDataUnit> CMyDatabase::m_RemoteData;
std::vector<RemoteDataUnit> CMyDatabase::m_NewAddRemoteData;
CString CMyDatabase::m_strLastError;
CString CMyDatabase::m_strDBLastTime;