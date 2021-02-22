#pragma once
#include "../common/function.h"
#include "baselib.h"
#include<errno.h>
#include <thread>
#include "work.h"

class LibEvtClient
	:public BaseLib::ISingleton<LibEvtClient>
	,public BaseLib::CTask
{
public:
	LibEvtClient():m_bConnect(FALSE){}
	~LibEvtClient();

	void SetConnect(BOOL bConnect);
	BOOL IsConnect();
	BOOL TryConnect();
	BOOL SetIPAndPort(string& strIP,int nPort);
	int InitSystem(std::string& strIP, int nPort);
	//�ϴ��ļ�
	int UpperFile(string strClass, string strStudent, string strCoures, string strFileName,string strFilePath, string strDeviceNum,int nType);
	//�����ļ���·��Ϊ��ʦ��·���������ݿ���鵽�ġ�nTemplate 1
    int DownloadFile(string strPath, string strCoures, int nType, int nTemplate);

	int GetFileList(string strClass, string strStudent, string& strOut);
    string GetDeviceNum(string strIP);

	string GetClass();
	string GetCourse();
	string GetClassStudent();
private:
	static void server_msg_cb(struct bufferevent* bev, void* arg);
	static void event_cb(struct bufferevent *bev, short event, void *arg);
	void SendToServer(std::string& strMsg);
	void Process(string strData);
	void SendFileToServer(string strData);
	INT32 Svc(void);
private:
	struct event_base * m_pBase;
	struct bufferevent* m_pBev;
	std::shared_ptr < std::thread > m_spListenThread;
	std::shared_ptr < CClientWorker > m_spWorker;

	BOOL m_bConnect;	//������
	string m_strIP;		//ip
	int m_nPort;		//�˿�
};
