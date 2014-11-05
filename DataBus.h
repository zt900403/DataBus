
//********************************************************************
//    DATABUS.H    �ļ�ע��
//    �ļ���  :    DATABUS.H
//    ����	 :    Tao Zhang
//    ����ʱ��:    2014/10/25 23:46
//    �ļ�����:	���ں���Ϣ����(DataBus)ͨ��,�ڰ�װ��Oracle Client�˺�
//				��ʼ��Ado��ǰ����,���Է������ݿ�(Oracle10g)���ж�ȡ��д�����.
//	Ado��ʼ��:
//	MFC��CXXApp��Initinstance������		if(!AfxOleInit())
//										{
//											AfxMessageBox("AfxOleInit()��ʼ��ʧ��!");
//										}
//
//	Win32 Console App��					CoInitialize();
//										�������ݿ����
//										CoUninitialize();	
//	��Ҫ�ڳ����Ŀ¼�´���һ�������Ľ�,�ļ���ΪConfig.ini,����������:
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
#define CONNECTION_TIMEOUT 5000																			//�������ݿⳬʱʱ�� ,��λ����
#define GETDATA_HASNEW	1
#define GETDATA_TRUE	0
#define GETDATA_FALSE	-1


//********************************************************************
//    CMyDataBus    ��ע��
//    ����    :    CMyDataBus
//  ��������:    NULL
//  �����ռ�:    NULL
//    ����    :    Tao Zhang
//    ����ʱ��:    2014/10/25 23:56
//    ������  :   ���ں�(DataBus)����ͨ�ŵ���,�ֱ�ʵ����:
//				1.bool getTask()			��ȡ����δ�������
//				2.bool getAllNewTask()		��ȡ����δ�������
//				3.bool setTaskState()		��������״̬
//				4.bool reportSystemInfo()	���ͱ���ϵͳ��Ϣ(����),ȷ������������ȷ
//*********************************************************************

//����ṹ��
struct TaskInfo{
	std::string strTaskName;
	std::string strTaskState;
	std::string strTaskMark;
	std::string strTaskReciMark;
	__int64 jlsj;//lMsgID;
};
class DATA_BUS_API CMyDataBus
{
//���캯��
public:
	CMyDataBus();
	~CMyDataBus();

//�ڲ���������
private:
	std::string EncodeUtf8(std::wstring in);		// ���ַ�תUTF8 ,�����ַ�����
	std::wstring DecodeUtf8(std::string in) ;		// UTF8 ת���ַ� 
	std::string ws2s(const std::wstring& ws);		//wstringתstring
	std::wstring s2ws(const std::string& s);		//string תwstring
	std::string getLocalTime();						//��ñ���ϵͳʱ��
	BOOL IsWinVersionEqualTo(DWORD dwMajorVersion, DWORD dwMinorVersion);		//�Ƚ�ϵͳ�汾,����x.x�汾
	BOOL IsWinVersionGreaterThan(DWORD dwMajorVersion, DWORD dwMinorVersion);	//�Ƚ�ϵͳ�汾,����x.x�汾
	BOOL IsWinVersionLessThan(DWORD dwMajorVersion, DWORD dwMinorVersion);		//�Ƚ�ϵͳ�汾,����x.x�汾
	char* getWindowsVersion(void);												//��ü�����汾
	__int64 CompareFileTime ( FILETIME time1, FILETIME time2 );					//�Ƚ�ϵͳʱ��,���ڼ���CPUʹ����
	int getCpuUsage();															//��ȡCPUʹ����
	bool writeErrorLog(char* czErrorContent,struct soap* soapClient=NULL);		//�����ӽṹ��soap�����Ĵ���д�������־,
//���ܺ���
public:
	bool getTask(int nTaskNumber);				//���ڻ�õ���δ�������,��Ҫ���������
	bool getAllNewTask();						//���ڻ������δ�������
	bool setTaskState(int nTaskNum,char* czTaskState);			//��������״̬
	bool reportSystemInfo();					//���ͱ���ϵͳ��Ϣ(����)


//����
public:


	struct TaskInfo		m_singleTaskInfo;		//�����ȡ����������ϸ��Ϣ�Ľṹ��
	std::vector<TaskInfo>	m_multiTaskInfo;	//�����ȡ���������ϸ��Ϣ��ʸ��
private:
	char m_czReciMark[TEMP_BUF_LEN];			//�����ļ���,��ȡ��ʶ
	char m_czErrorLog[TEMP_BUF_LEN];			//�����Ľ���;������־��ַ
	FILE* m_fpErrorLog;							//������־�ļ�ָ��
};



//********************************************************************
//    CMyDatabase    ��ע��
//    ����    :    CMyDatabase
//  ��������:    
//  �����ռ�:    NULL
//    ����    :    Tao Zhang
//    ����ʱ��:    2014/10/26 9:16
//    ������  :     �ڰ�װ��Oracle Client�ͻ��˺ͳ�ʼ��Ado��ǰ����,������
//					���ݿ�(Oracle10g)ͨ�ŵ���,�ֱ�ʵ����:
//					1.bool getData()		�����ݿ��л�ȡ����
//					2,bool writeData()		������д�����ݿ�
//*********************************************************************


//ң�����ݽṹ��
struct RemoteDataUnit{
	CString	strVariableName;				//������
	int				nFlag;						//���λ,0��ʾû����ֵ,1��ʾ����ֵ
	double			dValue;						//����ֵ
};
//typedef std::vector<RemoteDataUnit> v_RemoteData;		//�ⲿ��ȡ����ʸ������

class DATA_BUS_API CMyDatabase
{
//���캯��
public:
	CMyDatabase();
	~CMyDatabase();

//��������
private:
	static std::string getLocalTime();										//��ñ���ʱ��
	static bool writeErrorLog(char* czErrorContent,_com_error *e = NULL);	//������д�������־
	static bool connectToDBUntilLinked();									//�������ݿ�ֱ�����ӳɹ�(�ú����������߳�)
	static bool IsDisconnection();											//�ж������Ƿ�Ͽ�,����Ͽ�����true
	static BOOL getData(char* czTableName,char* czOrderBy="time");		//���ڻ�ȡ���ݿ�����,Ĭ�������ݿ���time�ֶν�������
	static bool getNewAddData(char*	czTableName,char* czOrderBy="time");	//��ȡ���ݿ�����������
	static unsigned int __stdcall getDataThreadFun(LPVOID lpvoid);			//��ȡ�����̺߳���;
	static unsigned int __stdcall getNewItemThreadFun(LPVOID lpvoid);		//��ȡ���ݿ���������Ŀ�̺߳���;

//���ܺ���
public:
	bool getRemoteData(RemoteDataUnit* __outAlterRemoteData,RemoteDataUnit* __outRemoteData=NULL);		//��ȡң������,__outRemoteDataVector��ϵ�����ң������,__outAlterRemoteDataVecotr���ϴε��ñ仯������
	bool writeData(char* czTableName,int nID,char* czContent);		//���ڽ�����д�����ݿ�
	CString getImageDirectory(char* czTableName,char* czIndex);	//��ȡͼ�������ļ���
	CString getLastError();										//������һ�δ�������
	void setLastError(char* czErrorInfo);		//���ô�����Ϣ
	bool beginToScanDB(char* czTableName,HWND hSendToMesssage=NULL);		//����ɨ���߳�
	bool CloseScanDBThread();					//�ر�ɨ���߳�
//����
public:

	
	static bool m_bDataThreadEndFlag;			//�߳̽�����־λ,trueΪ����,falseΪ������
	static HANDLE m_hScanDataThread;			//ɨ�������߳̾��

private:
	
	char m_czImageItemName[TEMP_BUF_LEN];	//�����ļ���;����ͼ�������ֶ���
	char m_czImagePathName[TEMP_BUF_LEN];	//�����ļ���;����ͼ�����ɵ�ַ�ֶ���
	
	static char m_czDBCon[TEMP_BUF_LEN];			//�����ļ���;���ݿ������ַ���	
	static int m_nLinkTestCount;					//���Ӳ��Դ���,ֻҪһ�γ���"ORA-03114: δ���ӵ� ORALCE"���������϶�Ϊ���ӶϿ�
	
	static CString m_strLastError;			//��¼���һ�δ�������
	static char m_czErrorLog[TEMP_BUF_LEN];		//�����ļ���;������־��ַ
	static FILE* m_fpErrorLog;						//������־�ļ�ָ��	
	static _ConnectionPtr	m_pDBConnection;		//���ݿ�����ָ��
	static char m_czMaxItemNum[TEMP_BUF_LEN];		//�����ļ���;�����ȡ����ʱ,���������������Ŀ����
	static std::vector<RemoteDataUnit> m_RemoteData;//��������ݿ��л�ȡ��ң�����ݵ�����
	static std::vector<RemoteDataUnit> m_NewAddRemoteData;	//�����ȡ��ң�����ݾ�ֵ
	static CRITICAL_SECTION m_g_cs;				//��֤m_RemoteData�ķ��ʻ���
	static HANDLE m_hSemaphoreProducer;			//�������ź���
	static HANDLE m_hSemaphoreConsumer;			//�������ź���
	static CString m_strDBLastTime;			//���ݿ���������Ŀ��ʱ��
	static HWND m_hSendToMessage;				//��ô��ڷ������ݿ������Ϣ
	
};

