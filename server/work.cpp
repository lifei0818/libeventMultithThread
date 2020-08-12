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
	case JLLJ: CommonTEST( content); break;
	default:break;
	}
}

void CServerWorker::excuteFile(string command, Json::Value& describeJson)
{
	switch (m_fileType[command]) {
	//case CSWJ: commandCSWJ(src); break;
	default: break;
	}
}

void CServerWorker::excuteFile(TCPSOURCETYPE src, string command)
{
	switch (m_fileType[command]) {
	//case CSWJ: commandCSWJ( src); break;
	default: break;
	}
}

void CServerWorker::initParameter()
{
	m_fileType["DWWJ"] = DWWJ;
	m_commandType["JLLJ"] = JLLJ;
}

void CServerWorker::SendToClient(string & strMsg)
{
	bufferevent_write(m_bev, strMsg.c_str(), strMsg.length());
}

void CServerWorker::CommonTEST(string & strContent)
{
}
