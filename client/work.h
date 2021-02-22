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

	//strData: �Ŀ͑����յ�����Ϣ 
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
	int SendMSQL(string strsql);		//�������ݿ�
	int SendXZWJ(string strsql);		//�����ļ�
	int SendWJLB(string strMsg);		//��ȡ�ļ��б�
    int SendSBBH(string strMsg);		//��ȡ�豸���
	//string GetServerResult();			//��ȡ����˷�������
	
    string GetFileList();
    string GetDeviceNumber();

	string GetClass();
	string GetCourse();
	string GetClassStudent();
private:
	std::map<std::string, FILE_TYPE_VALUE> m_fileType;
	std::map<std::string, COMMAND_TYPE_VALUE> m_commandType;

	string m_strFileList;				//����˷��ص��ļ��б�
    string m_strDeviceNum;

	string m_strClass;					//��ǰ�Ͽΰ༶
	string m_strCourse;					//��ǰ�γ�
	string m_strClassInfo;
};
