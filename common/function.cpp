#include "baselib.h"
#include "function.h"
#include <sstream>
#ifdef WIN32

#define DIRECTORY_ROOT "C:\\Files\\"
#define DIRECTORY_ROOT_UP  DIRECTORY_ROOT"upfile\\"
#define DIRECTORY_ROOT_DOWN DIRECTORY_ROOT"downfile\\"
bool WinSockInit() {
	WSADATA data = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &data))
		return false;
	if (LOBYTE(data.wVersion) != 2 || HIBYTE(data.wVersion) != 2) {
		WSACleanup();
		return false;
	}
	return true;
}
#else
#define DIRECTORY_ROOT "/tmp/Files/"
#define DIRECTORY_ROOT_UP  DIRECTORY_ROOT"upfile/"
#define DIRECTORY_ROOT_DOWN DIRECTORY_ROOT"downfile/"
#endif // WIN32

void log_callback(int severity, const char *msg)
{
	char szBuffer[512];

	FILE *pFd = fopen("./log.txt", "ab+");
	if (pFd == NULL)
		return;

	const char *severity_str;
	switch (severity)
	{
	case EVENT_LOG_DEBUG:
		severity_str = "debug";
		break;
	case EVENT_LOG_MSG:
		severity_str = "msg";
		break;
	case EVENT_LOG_WARN:
		severity_str = "warn";
		break;
	case EVENT_LOG_ERR:
		severity_str = "err";
		break;
	default:
		severity_str = "???";
		break;
	}

	snprintf(szBuffer, sizeof(szBuffer), "[%s]:%s\n", severity_str, msg);

	(void)fwrite(szBuffer, 1, strlen(szBuffer), pFd);

	fclose(pFd);
}

using namespace std;
int string_to_int(const std::string &p_content) {
	int m_res;
	if (p_content == "TRUE" || p_content == "ON") {
		m_res = 1;
	}
	else if (p_content == "FALSE" || p_content == "OFF") {
		m_res = 0;
	}
	else {
		stringstream ss;
		ss << p_content;
		ss >> m_res;
	}
	return m_res;
}

string num_to_string(int p_int) {
	stringstream ss;
	ss << p_int;
	return ss.str();
}
void CWorker::Process(string strData)
{
	bool nRt = false;
	//cout << "I am processing data that " << strData << GetCurrentThreadId() << endl;
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

				DBGPRINT(DBG_Libevt_RECV, new_package.label);
				excuteCommand(new_package.label, m_strMsgContent);
				m_strMsgContent.clear();
			}
            else if (new_package.type == TCPPACKAGE_FILE) {

				string strPath;
				string strContent;
				Json::Value fileJS;
				if (StringToJson(new_package.content, fileJS))
				{
					DBGPRINT(DBG_Libevt_RECV, ">>> recv file: " << new_package.label << ", " << new_package.num << "/" << new_package.allnum);
					string strPath(DIRECTORY_ROOT_UP);
					strPath += fileJS[FILE_DESCRIBE][FILE_CLASS].asString() + PATH_SEPARATOR;
					if (!BaseLib::IsFileExist(strPath.c_str()))
					{
						BaseLib::CxxCreateDirectory(strPath.c_str());
					}
#ifdef __SERVER
					strPath += fileJS[FILE_DESCRIBE][FILE_COURES].asString()+ "_"; // + ".program";
					strPath += fileJS[FILE_DESCRIBE][FILE_STUDENT].asString() ;					
					int nType = fileJS[FILE_DESCRIBE][FILE_TYPE].asInt();
					strPath += GetExtension(nType);
#else
					strPath += fileJS[FILE_DESCRIBE][FILE_NAME].asString();
#endif // __SERVER
					fileJS[FILE_DESCRIBE][FILE_PATH] = strPath;

                    if (new_package.num ==1&& BaseLib::IsFileExist(strPath.c_str())) {
                        BaseLib::CxxDeleteFile(strPath.c_str());
                    }
					if (!out.is_open()) {
						out.open(strPath.c_str(), ios::binary | ios::app);
						if (!out) {
                            setCMDFinishFlag(1);
							DBGPRINT(DBG_ERROR, "Open File failed:"<< strPath);
							break;
						}
						//out.seekg(0, ios::beg);
					}
					DBGPRINT(DBG_Libevt_RECV, "File:" << new_package.label << new_package.num << "/" << new_package.allnum << fileJS[FILE_CONTENT].asString().c_str());
					out.write(fileJS[FILE_CONTENT].asString().c_str(), fileJS[FILE_CONTENT].asString().length());
					out.close();
					if (new_package.num < new_package.allnum) {
						continue;
					}

					DBGPRINT(DBG_ERROR, new_package.label);
					excuteFile(new_package.label, fileJS[FILE_DESCRIBE]);
				}
			}
		}
		else
		{
			break;
		}
	}

}

void CWorker::SetBev(bufferevent * bev)
{
	m_bev = bev ;
}

void CWorker::SetIP(string & strIP)
{
	m_strIP = strIP;
}


string CWorker::GetIP()
{
	return m_strIP;
}

bool CWorker::GetPackage(PagTCP & new_package)
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


bool CWorker::StringToJson(string & strSoure, Json::Value & Root)
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

	DBGPRINT(DBG_Libevt_SEND, "json ½âÎö´íÎó:" << strSoure);
	return false;
}

string CWorker::JsonToString(Json::Value & Root)
{
	Json::StreamWriterBuilder builder;
	std::unique_ptr<Json::StreamWriter> writeInfo(builder.newStreamWriter());
	
	ostringstream oStr;
	writeInfo->write(Root, &oStr);
	return  oStr.str();
}

void CWorker::SendWCZL()
{
	SendMsgTo("WCZL", "1");
}
const char TeachProgramDirect[][256] = {"../robotTeachHR/config/robotProgram/","../robotTeachABB/robotTeachABBFile/RobotProgram.xml"};
string CWorker::GetDirectory(int nType,string strFileName)
{
    string strPath = BaseLib::GetModuleFullPath(TRUE)+PATH_SEP_STRING;
	if (nType > 2)
	{
		return "";
	}
    if(nType==0)
    {
        strPath+=TeachProgramDirect[nType];
        return strPath+strFileName;
    }
	return strPath+TeachProgramDirect[nType];
}

void CWorker::SendFileTo(const char* label, string & strMsg)
{
	string filePath;
	Json::Value fileJs;
	if (StringToJson(strMsg, fileJs))
	{
		filePath = fileJs[FILE_DESCRIBE][FILE_PATH].asString();
	}
	//m_tcp_mutex.lockMutex();
	char * buffer;
	int size;
	fstream in;
	in.open(filePath, ios::in | ios::binary | ios::ate);
	if (!in) {
		DBGPRINT(DBG_CLIENT_SEND, "Open File failed\n");
		return;
	}
	size = in.tellg();
	in.seekg(0, ios::beg);
	buffer = new char[size];
	in.read(buffer, size);
	in.close();
	string fileBuf(buffer);
	delete[] buffer;

	int time = size / PAGSIZE + 1;
	DBGPRINT(DBG_CLIENT_SEND, ">>> start to send file :" << label);
	Json::StreamWriterBuilder builder;
	builder.settings_["indentation"] = "";
	std::unique_ptr<Json::StreamWriter> writeInfo(builder.newStreamWriter());
	ostringstream oStr;
	for (int i = 0; i < time; i++) {
		int length = PAGSIZE;
		if (i == time - 1) {
			length = size % PAGSIZE;
		}
		PagTCP pag;
		pag.label = string(label);
		pag.src = TCPSOURCE_CLIENT;
		pag.type = TCPPACKAGE_FILE;
		pag.num = i + 1;
		pag.allnum = time;
		Json::Value contentJS;
		contentJS[FILE_DESCRIBE] = fileJs[FILE_DESCRIBE];
		string tmp = fileBuf.substr(i * PAGSIZE, length);
		contentJS[FILE_CONTENT] = tmp;
		//DBGPRINT(DBG_ERROR,"file "<<i<<"/"<<time<<"\t"<<tmp);
		oStr.str("");
		writeInfo->write(contentJS, &oStr);
		pag.content = oStr.str();

		if (send_package(pag) < 0) {
			return;
		}
	}

	DBGPRINT(DBG_CLIENT_SEND, ">>> finish sending file");

}

int CWorker::SendMsgTo(const char * label, string strMsg)
{
	DBGPRINT(DBG_Libevt_SEND, "send:"<<label);
	int size = strMsg.length();
	int time = size / PAGSIZE + 1;
	for (int i = 0; i < time; i++) {
		int length = PAGSIZE;
		if (i == time - 1) {
			length = size % PAGSIZE;
		}
		PagTCP pag;
		pag.label = string(label);
		pag.src = TCPSOURCE_SERVER;
		pag.type = TCPPACKAGE_MSG;
		pag.num = i + 1;
		pag.allnum = time;
		pag.content = strMsg.substr(i * PAGSIZE, length);

		if (send_package(pag) <= 0) {
			return -1;
		}
	}

	return 0;
}

int CWorker::send_package(PagTCP& pag) {
	string str = PAGHEAD;
	str += pag.label;
	str += num_to_string(pag.src) + SEGCHAR;
	str += num_to_string(pag.type) + SEGCHAR;
	str += num_to_string(pag.num) + SEGCHAR;
	str += num_to_string(pag.allnum) + SEGCHAR;
	str += pag.content;
	str += PAGTAIL;

	if (pag.type == TCPPACKAGE_MSG) {
		DBGPRINT(DBG_Libevt_SEND, ">>> send msg: " << pag.label << ", " << pag.num << "/" << pag.allnum << ", " << pag.content);
	}
	else if (pag.type == TCPPACKAGE_FILE) {
		DBGPRINT(DBG_Libevt_SEND, ">>> send file: " << pag.label << ", " << pag.num << "/" << pag.allnum << ", " << pag.content);
	}

	//if (send(fd, str.c_str(), str.length(), 0) < 0) {
	int nSend = SendMsgTo(str);
	if (nSend <= 0) {
		DBGPRINT(DBG_Libevt_SEND, "send TCP failed: " << errno);
		return -1;
	}
	DBGPRINT(DBG_Libevt_SEND, "nSend: " << nSend);

	return 0;
}

string CWorker::GetExtension(int nType)
{
	string strExt;
	switch (nType)
	{
	case 0:
		strExt = EXT_HR;
		break;
	case 1:
		strExt = EXT_ABB;
		break;
	case 2:
		strExt = EXT_FANUC;
		break;
	default:
		break;
	}
	return strExt;
}
int CWorker::SendMsgTo(string & strMsg)
{
	int nSend = send(bufferevent_getfd(m_bev), strMsg.c_str(), strMsg.length(), 0);
	return nSend;
}
void CWorker::setCMDFinishFlag(int flag) {
	m_cmdFinishFlag = flag;
	//sendWCZL(flag);
}

int CWorker::waitCMDFinishFlag() {
	while (1) {
		if (m_cmdFinishFlag >= 0) {
			return m_cmdFinishFlag;
		}
		BaseLib::OS::msleep(200);
	}
}
