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
	CWorker() : m_bev(NULL) {}
	~CWorker() {}
	void Process(string strDate) ;
	void SetBev(bufferevent * bev);
	void SendFileTo(const char* label, string & strMsg); 
	int SendMsgTo(string & strMsg);

	virtual void excuteCommand(std::string& command, std::string& content) = 0;
	virtual void excuteFile(string command, Json::Value& describeJson) = 0;
protected:
	bool StringToJson(string & strSoure, Json::Value & Root);
private:
	bool GetPackage(PagTCP& new_package);
	int send_package(PagTCP& pag);
protected:

	bufferevent * m_bev;	//socket事件
	string m_strData;	    //保存包数据
	string m_strMsgContent;	//保存消息信息
};
