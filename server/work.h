#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include "../common/dataStructure.h"
using namespace std;
class CWorker {
public:
	CWorker() {}
	~CWorker() {}
	virtual void Process(string strDate) = 0;
protected:
	string m_strData;	//保存包数据
	string m_strMsgContent;	//保存消息信息
};

class CServerWorker
	: public CWorker
{
public:
	CServerWorker();
	~CServerWorker();

	//strData: 從客戶端收到的消息 
	void Process(string strData);
	void SetBev(bufferevent * bev);
private:
	bool GetPackage(PagTCP& new_package);
	void excuteCommand(std::string& command, std::string& content);
	void excuteFile(string command, Json::Value& describeJson);
	void excuteFile(TCPSOURCETYPE src, string command);
	bool StringToJson(string& strSoure, Json::Value& Root);
	void initParameter();
	void SendToClient(string& strMsg);

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
	bufferevent * m_bev;
	std::map<std::string, FILE_TYPE_VALUE> m_fileType;
	std::map<std::string, COMMAND_TYPE_VALUE> m_commandType;
};
