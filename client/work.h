#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include "function.h"
#include "../common/dataStructure.h"
using namespace std;


class CClientWorker
	: public CWorker
{
public:
	CClientWorker();
	~CClientWorker();

	//strData: 從客戶端收到的消息 
	//void Process(string strData);
	//void SetBev(bufferevent * bev);
	//void SendFileToServer(const char* lable, string& strMsg);
	void excuteCommand(std::string& command, std::string& content);
	void excuteFile(string command, Json::Value& describeJson);
private:
	void excuteFile(TCPSOURCETYPE src, string command);
	void initParameter();
	int SendToServer(string& strMsg);

	//int send_package(PagTCP& pag);
	enum FILE_TYPE_VALUE { CSWJ, MLWJ, PZWJ, ZBWJ, DWWJ };
	enum COMMAND_TYPE_VALUE {
		WCZL, MSQL, 
		TEST
	};
	/////////////////////////////////////////////////////////////////////////
	void CommonTEST(string& strContent );
	void CommonWCZL(string& strContent);
	void CommonMSQL(string& strContent);
	/////////////////////////////////////////////////////////////////////////
	void SendMSQL(string strsql);
private:
	std::map<std::string, FILE_TYPE_VALUE> m_fileType;
	std::map<std::string, COMMAND_TYPE_VALUE> m_commandType;
};
