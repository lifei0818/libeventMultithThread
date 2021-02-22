#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include "../common/function.h"
#include "../common/dataStructure.h"
#include "StudentManager.h"
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
        WCZL, MSQL, WJLB,XZWJ,SBBH,
		TEST
	};

	void CommanCXWJ(Json::Value& strContent);	//程序文件

	void CommanTEST(string& strContent );
	void CommanWCZL(string& strContent);	
	void CommanMSQL(string& strContent);
	void CommanXZWJ(string& strContent);	//下载文件
	void CommanWJLB(string& strContent);	//文件列表
    void CommanSBBH(string& strContent);	//设备编号
public:
	void SendCXWJ(string& strContent);
	void SendSKBJ(string& strClass, string& strCourse);		//上课班级
private:
	std::map<std::string, FILE_TYPE_VALUE> m_fileType;
	std::map<std::string, COMMAND_TYPE_VALUE> m_commandType;

	std::shared_ptr < StudentManager > m_spStudentMan;
};
