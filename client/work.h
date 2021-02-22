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
        WCZL, MSQL, WJLB, SBBH, SKBJ,
		TEST
	};
	/////////////////////////////////////////////////////////////////////////
	void CommanTEST(string& strContent );
	void CommanWCZL(string& strContent);
	void CommanMSQL(string& strContent);
	void CommanWJLB(string& strContent);
    void CommanSBBH(string& strContent);
	void CommanSKBJ(string& strContent);


	void CommanCXWJ(Json::Value& describeJson);
public:
	/////////////////////////////////////////////////////////////////////////
	//return 0:success <0:false
	int SendMSQL(string strsql);		//访问数据库
	int SendXZWJ(string strsql);		//下载文件
	int SendWJLB(string strMsg);		//获取文件列表
    int SendSBBH(string strMsg);		//获取设备编号
	//string GetServerResult();			//获取服务端返回数据
	
    string GetFileList();
    string GetDeviceNumber();

	string GetClass();
	string GetCourse();
	string GetClassStudent();
private:
	std::map<std::string, FILE_TYPE_VALUE> m_fileType;
	std::map<std::string, COMMAND_TYPE_VALUE> m_commandType;

	string m_strFileList;				//服务端返回的文件列表
    string m_strDeviceNum;

	string m_strClass;					//当前上课班级
	string m_strCourse;					//当前课程
	string m_strClassInfo;
};
