#pragma once
#include "../common/function.h"
#include "baselib.h"
#include<errno.h>
#include <thread>
#include "work.h"

//int tcp_connect_server(const char* server_ip, int port);
//int client(int argc, char** argv);
//void cmd_msg_cb(evutil_socket_t fd, short events, void* arg);
//
//void socket_read_cb(intptr_t fd, short events, void *arg);
//int client2(int argc, char** argv);
class LibEvtClient {
public:
	LibEvtClient() {}
	~LibEvtClient() {}

	int InitSystem(std::string& strIP, int nPort);
	//�ϴ��ļ�
	void UpperFile(string strClass, string strStudent, string strCoures, string strFileName,string strFilePath, string strDeviceNum,int nType);
	//�����ļ���·��Ϊ��ʦ��·���������ݿ���鵽�ġ�nTemplate 1
	void DownloadFile(string strPath, string strCoures, int nType, int nTemplate);

	void GetFileList(string strClass, string strStudent, string& strOut);
private:
	static void server_msg_cb(struct bufferevent* bev, void* arg);
	static void event_cb(struct bufferevent *bev, short event, void *arg);
	void SendToServer(std::string& strMsg);
	void Process(string strData);
	void SendFileToServer(string strData);
private:
	struct event_base * m_pBase;
	struct bufferevent* m_pBev;
	std::shared_ptr < std::thread > m_spListenThread;
	std::shared_ptr < CClientWorker > m_spWorker;
};