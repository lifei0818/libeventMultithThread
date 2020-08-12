#include "work.h"
#include <windows.h>
CServerWorker::CServerWorker()
	:m_bev(NULL)
{
}

CServerWorker::~CServerWorker()
{
}

void CServerWorker::Process(string strData)
{
	bool nRt = false;
	cout << "I am processing data that " << strData <<GetCurrentThreadId() << endl;
	m_strData += strData;
	fstream out;
	while (true)
	{
		PagTCP new_package;
		if (GetPackage(new_package))
		{
			if (new_package.type == TCPPACKAGE_MSG) {
				m_strMsgContent += new_package.content;
				if (new_package.num < new_package.allnum) {
					continue;
				}

				excuteCommand( new_package.label, m_strMsgContent);
				m_strMsgContent.clear();
			}
			else if (new_package.type == TCPPACKAGE_FILE) {
				string strPath;
				string strContent;
				Json::Value fileJS;
				if (StringToJson(new_package.content, fileJS))
				{
					DBGPRINT(DBG_CLIENT_RECV, ">>> recv file: " << new_package.label << ", " << new_package.num << "/" << new_package.allnum);
					if (!out.is_open()) {
						out.open((fileJS[FILE_DESCRIBE][FILE_PATH].asString()).c_str(), ios::out | ios::binary | ios::ate);
						if (!out) {
							DBGPRINT(DBG_CLIENT_RECV, "Open File failed");
							break;
						}
						out.seekg(0, ios::beg);
					}

					out.write(fileJS[FILE_CONTENT].asString().c_str(), fileJS[FILE_CONTENT].asString().length());

					if (new_package.num < new_package.allnum) {
						continue;
					}

					//DBGPRINT(DBG_CLIENT_RECV, "file from server is received:" << accept_fd);
					out.close();
					excuteFile( new_package.label,fileJS[FILE_DESCRIBE]);
				}
			}
		}
		else
		{
			break;
		}
	}

}

void CServerWorker::SetBev(bufferevent * bev)
{
	m_bev = bev;
}

bool CServerWorker::GetPackage(PagTCP & new_package)
{
	string packageStr = m_strData;
	int index = packageStr.find(PAGHEAD);
	if (index >= 0) {
		packageStr = packageStr.substr(index);
		index = packageStr.find(PAGTAIL);
		if (index >= 0) {
			m_strData = packageStr.substr(index + string(PAGTAIL).length());
			packageStr = packageStr.substr(string(PAGHEAD).length(), index - string(PAGHEAD).length());
		
		}
		else
		{
			return false;
		}
		new_package.label = packageStr.substr(0, 4);
		packageStr = packageStr.substr(4);

		int index;
		index = packageStr.find(SEGCHAR);
		new_package.src = TCPSOURCETYPE(string_to_int(packageStr.substr(0, index)));
		packageStr = packageStr.substr(index + 1);
		index = packageStr.find(SEGCHAR);
		new_package.type = TCPPACKAGETYPE(string_to_int(packageStr.substr(0, index)));
		packageStr = packageStr.substr(index + 1);
		index = packageStr.find(SEGCHAR);
		new_package.num = string_to_int(packageStr.substr(0, index));
		packageStr = packageStr.substr(index + 1);
		index = packageStr.find(SEGCHAR);
		new_package.allnum = string_to_int(packageStr.substr(0, index));
		packageStr = packageStr.substr(index + 1);

		new_package.content = packageStr;

		return true;
	}
	return false;
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

bool CServerWorker::StringToJson(string & strSoure, Json::Value & Root)
{
	if (!strSoure.empty())
	{
		Json::CharReaderBuilder builder;
		std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
		string err;
		if (reader->parse(strSoure.c_str(), strSoure.c_str() + strSoure.length(), &Root, &err))
		{
			return true;
		}
	}

	return false;
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