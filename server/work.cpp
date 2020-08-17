#include "work.h"
#include "StudentManager.h"
#ifdef WIN32
#include <windows.h>
#endif
CServerWorker::CServerWorker()
{
	m_spStudentMan.reset(new StudentManager());
	initParameter();
}

CServerWorker::~CServerWorker()
{
}


void CServerWorker::excuteCommand(std::string& command, std::string& content)
{
	switch (m_commandType[command]) {
	case WCZL: CommanWCZL(content); break;
	case MSQL: CommanMSQL(content); break;
	case WJLB: CommanWJLB( content); break; 
    case XZWJ: CommanXZWJ(content); break;
    case SBBH: CommanSBBH(content); break;
	case TEST: CommanTEST(content); break;
	default:break;
	}
}

void CServerWorker::excuteFile(string command, Json::Value& describeJson)
{
	switch (m_fileType[command]) {
	case CXWJ: CommanCXWJ(describeJson); break;
	default: break;
	}
}

void CServerWorker::excuteFile(TCPSOURCETYPE src, string command)
{
	switch (m_fileType[command]) {
	//case CXWJ: CommanCXWJ( src); break;
	default: break;
	}
}

void CServerWorker::initParameter()
{
	m_fileType["CXWJ"] = CXWJ;
	m_commandType["WCZL"] = WCZL;
	m_commandType["MSQL"] = MSQL;
	m_commandType["XZWJ"] = XZWJ;
    m_commandType["SBBH"] = SBBH;

	m_commandType["WJLB"] = WJLB;
}

void CServerWorker::SendToClient(string & strMsg)
{
	bufferevent_write(m_bev, strMsg.c_str(), strMsg.length());
}

void CServerWorker::CommanCXWJ(Json::Value & fileJson)
{
	//写文件信息到数据库
	Json::StreamWriterBuilder builder;
	builder.settings_["indentation"] = "";
	std::unique_ptr<Json::StreamWriter> writeInfo(builder.newStreamWriter());
	ostringstream oStr;
	writeInfo->write(fileJson, &oStr);
	string strMsg = oStr.str();
	StudentManager man;
	man.UpFile(strMsg);
}

void CServerWorker::CommanTEST(string & strContent)
{
}

void CServerWorker::CommanWCZL(string & strContent)
{
	setCMDFinishFlag(1);
	SendWCZL();
}

void CServerWorker::CommanMSQL(string & strContent)
{
	//执行Mysql语句
	BaseLib::TSingleton<DataHelper>::Instance()->excuteSql(strContent);
	SendWCZL();
}

void CServerWorker::CommanXZWJ(string& strContent)
{
	Json::Value fileJson;
	if (StringToJson(strContent, fileJson))
	{
		string strPath = fileJson[FILE_DESCRIBE][FILE_PATH].asString();
		BOOL bRt = BaseLib::IsFileExist(strPath.c_str());
		if (bRt)
		{
			SendCXWJ(strContent);
		}
	}

	//SendWCZL();
}

void CServerWorker::CommanWJLB(string & strContent)
{
	Json::Value msgJs;
	if (StringToJson(strContent, msgJs))
	{
		string strClass = msgJs[FILE_CLASS].asString();//msgJs[FILE_LIST]
		string strStudent = msgJs[FILE_STUDENT].asString();
		string strFileList;
		m_spStudentMan->GetFiles(strClass,strStudent,strFileList);
		msgJs[FILE_LIST] = strFileList;
		SendMsgTo("WJLB", JsonToString(msgJs));
	}
}


void CServerWorker::CommanSBBH(string& strContent){
    //Device(string& strDevice, int op= 0);
    string str("{\"ipaddr\":\"\",\"deviceNumber\" : \"");
    str += strContent + "\"}";
    m_spStudentMan->Device(str,2);
    SendMsgTo("SBBH",str);
}

void CServerWorker::SendCXWJ(string& strContent)
{
	SendFileTo("CXWJ", strContent);
}
