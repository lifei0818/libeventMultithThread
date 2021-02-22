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

	void CommanCXWJ(Json::Value& strContent);	//�����ļ�

	void CommanTEST(string& strContent );
	void CommanWCZL(string& strContent);	
	void CommanMSQL(string& strContent);
	void CommanXZWJ(string& strContent);	//�����ļ�
	void CommanWJLB(string& strContent);	//�ļ��б�
    void CommanSBBH(string& strContent);	//�豸���
public:
	void SendCXWJ(string& strContent);
	void SendSKBJ(string& strClass, string& strCourse);		//�Ͽΰ༶
private:
	std::map<std::string, FILE_TYPE_VALUE> m_fileType;
	std::map<std::string, COMMAND_TYPE_VALUE> m_commandType;

	std::shared_ptr < StudentManager > m_spStudentMan;
};
