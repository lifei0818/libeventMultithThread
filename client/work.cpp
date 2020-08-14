#include "work.h"
//#include <windows.h>
CClientWorker::CClientWorker()
{
	initParameter();
}

CClientWorker::~CClientWorker()
{
}

void CClientWorker::excuteCommand(std::string& command, std::string& content)
{
	switch (m_commandType[command]) {
	case WCZL: CommanWCZL( content); break;
	case MSQL: CommanMSQL(content); break;
	case WJLB: CommanWJLB(content); break;
	default:break;
	}
}

void CClientWorker::excuteFile(string command, Json::Value& describeJson)
{
	switch (m_fileType[command]) {
	case CXWJ: CommanCXWJ(describeJson); break;
	default: break;
	}
}


void CClientWorker::initParameter()
{
	m_fileType["CXWJ"] = CXWJ;
	m_commandType["WCZL"] = WCZL;
	m_commandType["MSQL"] = MSQL;
	m_commandType["WJLB"] = WJLB;
}

int CClientWorker::SendToServer(string & strMsg)
{
	//bufferevent_write(m_bev, strMsg.c_str(), strMsg.length());
	return send(bufferevent_getfd(m_bev), strMsg.c_str(), strMsg.length(), 0);
}

void CClientWorker::CommanTEST(string & strContent)
{
}

void CClientWorker::CommanWCZL(string & strContent)
{
	setCMDFinishFlag(1);
}

void CClientWorker::CommanMSQL(string & strContent)
{
	//存储服务端sql返回的数据

}
void CClientWorker::CommanWJLB(string & strContent)
{
	Json::Value fileListJs;
	if (StringToJson(strContent, fileListJs))
	{
		m_strFileList = fileListJs[FILE_LIST].asString();
	}
	setCMDFinishFlag(1);
}
void CClientWorker::CommanCXWJ(Json::Value& describeJson)
{
	//将文件剪切到示教器目录下
	int nType = describeJson[FILE_TYPE].asInt();
	string strSrc = describeJson[FILE_PATH].asString();
	string strDir = GetDirectory(nType)+PATH_SEP_STRING;
	
	BOOL bRt = BaseLib::CxxCopyFile(strSrc.c_str(), strDir.c_str(),FALSE);
	if(!bRt)
	{
		DBGPRINT(DBG_ERROR,"CxxCopyFile false! \nsrc:"<<strSrc<<"\ndes:"<<strDir);
	}
	setCMDFinishFlag(1);
}

void CClientWorker::SendMSQL(string strsql)
{
	setCMDFinishFlag(-1);
	SendMsgTo("MSQL", strsql);
	waitCMDFinishFlag();
}

void CClientWorker::SendXZWJ(string strsql)
{
	setCMDFinishFlag(-1);
	SendMsgTo("XZWJ", strsql);
	waitCMDFinishFlag();
}

void CClientWorker::SendWJLB(string strMsg)
{
	SendMsgTo("WJLB", strMsg);
}

string CClientWorker::GetFileList()
{
	return m_strFileList;
}
