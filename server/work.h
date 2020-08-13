#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include "../common/function.h"
#include "../common/dataStructure.h"
using namespace std;

class CServerWorker
	: public CWorker
{
public:
	CServerWorker();
	~CServerWorker();

	void excuteCommand(std::string& command, std::string& content);
	void excuteFile(string command, Json::Value& describeJson);
private:
	void excuteFile(TCPSOURCETYPE src, string command);
	void initParameter();
	void SendToClient(string& strMsg);

	enum FILE_TYPE_VALUE { CXWJ};
	enum COMMAND_TYPE_VALUE {
		WCZL, MSQL, 
		TEST
	};

	void CommonCXWJ(Json::Value& strContent);

	void CommonTEST(string& strContent );
	void CommonWCZL(string& strContent);
	void CommonMSQL(string& strContent);
private:
	std::map<std::string, FILE_TYPE_VALUE> m_fileType;
	std::map<std::string, COMMAND_TYPE_VALUE> m_commandType;
};
