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
		JLLJ, GBLJ, QCCW, CXSD, SJXH, SJWZ, HLJZ, XWHL,
		SCZT, MNSC, MNDL, CSYX, FSZT, SFZT, TSZT, YXMS, KZXT,
		XGGJ, XGYH, TCPF, TCFZ, TCFX, USRF,
		GJDD, MDDD, DDTZ, GJBJ, MDBJ, GJYX, MLYX, MLZT, MLJX, MLTZ, DBYX, DBZT, CXXH,
		KSTD, JSTD, JLTD, ZXZB, ZXTD, SZLX, BEEP, SZYY, MBBT, MBRG, XGIP, INIT,
		TEST
	};

	void CommonTEST(string& strContent );
private:
	std::map<std::string, FILE_TYPE_VALUE> m_fileType;
	std::map<std::string, COMMAND_TYPE_VALUE> m_commandType;
};
