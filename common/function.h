#pragma once

#include<stdio.h>  
#include<string.h>  
#include<string>
#include<event.h>
#include<event2/util.h>
#include<event2/buffer.h>
#include <event2/thread.h>
#include<event2/listener.h> 
#include<event2/bufferevent.h>
#include "dataStructure.h"
#ifdef WIN32
bool WinSockInit();
#endif // WIN32

using namespace std;

void log_callback(int severity, const char *msg);
int string_to_int(const std::string &p_content);
string num_to_string(int p_int);


class CWorker {
public:
	CWorker() : m_bev(NULL), m_cmdFinishFlag(-1){}
	~CWorker() {}
	void Process(string strDate) ;		//����socket���յ�������
	void SetBev(bufferevent * bev);		//����socket�¼�ָ��
	void SendFileTo(const char* label, string & strMsg);	//��ʽ�����ļ�
	int SendMsgTo(const char* label, string strMsg);		//��ʽ������Ϣ
	int SendMsgTo(string & strMsg);							//�޸�ʽ������Ϣ
	void setCMDFinishFlag(int flag);	//���õȴ���ʼֵ
	int waitCMDFinishFlag();		//�ȴ�ָ���

	virtual void excuteCommand(std::string& command, std::string& content) = 0;	//��������ָ��
	virtual void excuteFile(string command, Json::Value& describeJson) = 0;
protected:
	bool StringToJson(string & strSoure, Json::Value & Root);
	string JsonToString(Json::Value & Root);
	void SendWCZL();
    string GetDirectory(int nType,string strFileName);
private:
	bool GetPackage(PagTCP& new_package);
	int send_package(PagTCP& pag);

	string GetExtension(int nType);

	int m_cmdFinishFlag;
protected:

	bufferevent * m_bev;	//socket�¼�
	string m_strData;	    //���������
	string m_strMsgContent;	//������Ϣ��Ϣ
};
