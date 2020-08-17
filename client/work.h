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
	void initParameter();
	int SendToServer(string& strMsg);

	//int send_package(PagTCP& pag);
	enum FILE_TYPE_VALUE { CXWJ };
	enum COMMAND_TYPE_VALUE {
        WCZL, MSQL, WJLB, SBBH,
		TEST
	};
	/////////////////////////////////////////////////////////////////////////
	void CommanTEST(string& strContent );
	void CommanWCZL(string& strContent);
	void CommanMSQL(string& strContent);
	void CommanWJLB(string& strContent);
    void CommanSBBH(string& strContent);


	void CommanCXWJ(Json::Value& describeJson);
public:
	/////////////////////////////////////////////////////////////////////////
	void SendMSQL(string strsql);		//访问数据库
	void SendXZWJ(string strsql);		//下载文件
	void SendWJLB(string strMsg);		//获取文件列表
    void SendSBBH(string strMsg);		//获取设备编号
	//string GetServerResult();			//获取服务端返回数据
	
    string GetFileList();
    string GetDeviceNumber();

private:
	std::map<std::string, FILE_TYPE_VALUE> m_fileType;
	std::map<std::string, COMMAND_TYPE_VALUE> m_commandType;

	string m_strFileList;				//服务端返回的文件列表
    string m_strDeviceNum;
};
