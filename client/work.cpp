#include "work.h"
//#include <windows.h>
CClientWorker::CClientWorker()
{
}

CClientWorker::~CClientWorker()
{
}

//void CClientWorker::Process(string strData)
//{
//	bool nRt = false;
//	cout << "I am processing data that " << strData /*<<GetCurrentThreadId()*/ << endl;
//	m_strData += strData;
//	fstream out;
//	while (true)
//	{
//		PagTCP new_package;
//		if (GetPackage(new_package))
//		{
//			if (new_package.type == TCPPACKAGE_MSG) {
//				m_strMsgContent += new_package.content;
//				if (new_package.num < new_package.allnum) {
//					continue;
//				}
//
//				excuteCommand( new_package.label, m_strMsgContent);
//				m_strMsgContent.clear();
//			}
//			else if (new_package.type == TCPPACKAGE_FILE) {
//				string strPath;
//				string strContent;
//				Json::Value fileJS;
//				if (StringToJson(new_package.content, fileJS))
//				{
//					DBGPRINT(DBG_CLIENT_RECV, ">>> recv file: " << new_package.label << ", " << new_package.num << "/" << new_package.allnum);
//					if (!out.is_open()) {
//						out.open((fileJS[FILE_DESCRIBE][FILE_PATH].asString()).c_str(), ios::out | ios::binary | ios::ate);
//						if (!out) {
//							DBGPRINT(DBG_CLIENT_RECV, "Open File failed");
//							break;
//						}
//						out.seekg(0, ios::beg);
//					}
//
//					out.write(fileJS[FILE_CONTENT].asString().c_str(), fileJS[FILE_CONTENT].asString().length());
//					//DBGPRINT(DBG_CLIENT_RECV, "file from server is received:" << accept_fd);
//					out.close();
//					if (new_package.num < new_package.allnum) {
//						continue;
//					}
//
//					excuteFile( new_package.label,fileJS[FILE_DESCRIBE]);
//				}
//			}
//		}
//		else
//		{
//			break;
//		}
//	}
//
//}
//
//void CClientWorker::SetBev(bufferevent * bev)
//{
//	m_bev = bev;
//}

//bool CClientWorker::GetPackage(PagTCP & new_package)
//{
//	string packageStr = m_strData;
//	int index = packageStr.find(PAGHEAD);
//	if (index >= 0) {
//		packageStr = packageStr.substr(index);
//		index = packageStr.find(PAGTAIL);
//		if (index >= 0) {
//			m_strData = packageStr.substr(index + string(PAGTAIL).length());
//			packageStr = packageStr.substr(string(PAGHEAD).length(), index - string(PAGHEAD).length());
//		
//		}
//		else
//		{
//			return false;
//		}
//		new_package.label = packageStr.substr(0, 4);
//		packageStr = packageStr.substr(4);
//
//		int index;
//		index = packageStr.find(SEGCHAR);
//		new_package.src = TCPSOURCETYPE(string_to_int(packageStr.substr(0, index)));
//		packageStr = packageStr.substr(index + 1);
//		index = packageStr.find(SEGCHAR);
//		new_package.type = TCPPACKAGETYPE(string_to_int(packageStr.substr(0, index)));
//		packageStr = packageStr.substr(index + 1);
//		index = packageStr.find(SEGCHAR);
//		new_package.num = string_to_int(packageStr.substr(0, index));
//		packageStr = packageStr.substr(index + 1);
//		index = packageStr.find(SEGCHAR);
//		new_package.allnum = string_to_int(packageStr.substr(0, index));
//		packageStr = packageStr.substr(index + 1);
//
//		new_package.content = packageStr;
//
//		return true;
//	}
//	return false;
//}
//
void CClientWorker::excuteCommand(std::string& command, std::string& content)
{
	switch (m_commandType[command]) {
	case JLLJ: CommonTEST( content); break;
	default:break;
	}
}

void CClientWorker::excuteFile(string command, Json::Value& describeJson)
{
	switch (m_fileType[command]) {
	//case CSWJ: commandCSWJ(src); break;
	default: break;
	}
}

void CClientWorker::excuteFile(TCPSOURCETYPE src, string command)
{
	switch (m_fileType[command]) {
	//case CSWJ: commandCSWJ( src); break;
	default: break;
	}
}

//bool CClientWorker::StringToJson(string & strSoure, Json::Value & Root)
//{
//	if (!strSoure.empty())
//	{
//		Json::CharReaderBuilder builder;
//		std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
//		string err;
//		if (reader->parse(strSoure.c_str(), strSoure.c_str() + strSoure.length(), &Root, &err))
//		{
//			return true;
//		}
//	}
//
//	return false;
//}

void CClientWorker::initParameter()
{
	m_fileType["DWWJ"] = DWWJ;
	m_commandType["JLLJ"] = JLLJ;
}

int CClientWorker::SendToServer(string & strMsg)
{
	//bufferevent_write(m_bev, strMsg.c_str(), strMsg.length());
	return send(bufferevent_getfd(m_bev), strMsg.c_str(), strMsg.length(), 0);
}

//void TCPClient::send_file(const char *label, const char *filePath) {
//void CClientWorker::SendFileToServer(const char* label,string & strMsg)
//{
//	string filePath;
//	Json::Value fileJs;
//	if (StringToJson(strMsg, fileJs))
//	{
//		filePath = fileJs[FILE_DESCRIBE][FILE_PATH].asString();
//	}
//	//m_tcp_mutex.lockMutex();
//	char * buffer;
//	int size;
//	fstream in;
//	in.open(filePath, ios::in | ios::binary | ios::ate);
//	if (!in) {
//		DBGPRINT(DBG_CLIENT_SEND, "Open File failed\n");
//		return;
//	}
//	size = in.tellg();
//	in.seekg(0, ios::beg);
//	buffer = new char[size];
//	in.read(buffer, size);
//	in.close();
//	string fileBuf(buffer);
//	delete[] buffer;
//
//	int time = size / PAGSIZE + 1;
//	DBGPRINT(DBG_CLIENT_SEND, ">>> start to send file :" << label);
//	Json::StreamWriterBuilder builder;
//	builder.settings_["indentation"] = "";
//	std::unique_ptr<Json::StreamWriter> writeInfo(builder.newStreamWriter());
//	ostringstream oStr;
//	for (int i = 0; i < time; i++) {
//		int length = PAGSIZE;
//		if (i == time - 1) {
//			length = size % PAGSIZE;
//		}
//		PagTCP pag;
//		pag.label = string(label);
//		pag.src = TCPSOURCE_CLIENT;
//		pag.type = TCPPACKAGE_FILE;
//		pag.num = i + 1;
//		pag.allnum = time;
//		Json::Value contentJS;
//		contentJS[FILE_DESCRIBE] = fileJs[FILE_DESCRIBE];
//		contentJS[FILE_CONTENT] = fileBuf.substr(i * PAGSIZE, length);
//		oStr.str("");
//		writeInfo->write(contentJS, &oStr);
//		pag.content = oStr.str();
//
//		if (send_package( pag) < 0) {
//			return;
//		}
//	}
//
//	DBGPRINT(DBG_CLIENT_SEND, ">>> finish sending file");
//	
//}

//int CClientWorker::send_package( PagTCP& pag) {
//	string str = PAGHEAD;
//	str += pag.label;
//	str += num_to_string(pag.src) + SEGCHAR;
//	str += num_to_string(pag.type) + SEGCHAR;
//	str += num_to_string(pag.num) + SEGCHAR;
//	str += num_to_string(pag.allnum) + SEGCHAR;
//	str += pag.content;
//	str += PAGTAIL;
//
//	if (pag.type == TCPPACKAGE_MSG) {
//		DBGPRINT(DBG_CLIENT_SEND, ">>> send msg: " << pag.label << ", " << pag.num << "/" << pag.allnum << ", " << pag.content);
//	}
//	else if (pag.type == TCPPACKAGE_FILE) {
//		DBGPRINT(DBG_CLIENT_SEND, ">>> send file: " << pag.label << ", " << pag.num << "/" << pag.allnum << ", " << pag.content);
//	}
//
//	//if (send(fd, str.c_str(), str.length(), 0) < 0) {
//	if(SendToServer(str)<0){
//		DBGPRINT(DBG_CLIENT_SEND, "send TCP failed: " << errno);
//		return -1;
//	}
//
//	return 0;
//}

void CClientWorker::CommonTEST(string & strContent)
{
}
