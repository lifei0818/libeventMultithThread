#include "work.h"
#ifdef WIN32
#include <windows.h>
#endif
CServerWorker::CServerWorker()
{
}

CServerWorker::~CServerWorker()
{
}


void CServerWorker::excuteCommand(std::string& command, std::string& content)
{
	switch (m_commandType[command]) {
	case WCZL: CommonWCZL(content); break;
	case MSQL: CommonMSQL(content); break;
	case TEST: CommonTEST( content); break;
	default:break;
	}
}

void CServerWorker::excuteFile(string command, Json::Value& describeJson)
{
	switch (m_fileType[command]) {
	case CXWJ: CommonCXWJ(describeJson); break;
	default: break;
	}
}

void CServerWorker::excuteFile(TCPSOURCETYPE src, string command)
{
	switch (m_fileType[command]) {
	//case CXWJ: CommonCXWJ( src); break;
	default: break;
	}
}

void CServerWorker::initParameter()
{
	m_fileType["CXWJ"] = CXWJ;
	m_commandType["WCZL"] = WCZL;
	m_commandType["MSQL"] = MSQL;
}

void CServerWorker::SendToClient(string & strMsg)
{
	bufferevent_write(m_bev, strMsg.c_str(), strMsg.length());
}

void CServerWorker::CommonCXWJ(Json::Value & strContent)
{
	//写文件信息到数据库
}

void CServerWorker::CommonTEST(string & strContent)
{
}

void CServerWorker::CommonWCZL(string & strContent)
{
	setCMDFinishFlag(1);
}

void CServerWorker::CommonMSQL(string & strContent)
{
	//执行Mysql语句
}
