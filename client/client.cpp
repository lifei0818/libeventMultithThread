#include "client.h"
#include<errno.h>

#ifdef WIN32
#include <windows.h>
#include <Winsock2.h>
#include <process.h>
#else
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#endif // WIN32
#include <string>

LibEvtClient::~LibEvtClient() {
	if (m_pBev) {
		bufferevent_free(m_pBev);
	}

	if (m_pBase){
		event_base_loopbreak(m_pBase);
		BaseLib::OS::sleep(1);
		event_base_free(m_pBase);
	}
	m_spListenThread->join();

}

void LibEvtClient::SetConnect(BOOL bConnect)
{
	m_bConnect = bConnect;
}
BOOL LibEvtClient::IsConnect()
{
	return m_bConnect;
}

BOOL LibEvtClient::TryConnect()
{
	if (m_bConnect)
	{
		return TRUE;
	}
	if (m_pBev)
	{
		bufferevent_free(m_pBev);
	}
	if (m_pBase)
	{
		event_base_free(m_pBase);
	}

	m_pBase = event_base_new();
	m_pBev = bufferevent_socket_new(m_pBase, -1,
		BEV_OPT_CLOSE_ON_FREE);
	
	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(m_nPort);
	inet_pton(AF_INET, m_strIP.c_str(), &server_addr.sin_addr);

	int nRt = -1;
	nRt = bufferevent_socket_connect(m_pBev, (struct sockaddr *)&server_addr,
		sizeof(server_addr));
	if (nRt == -1)
	{
		DBGPRINT(DBG_ERROR, "connect false! bufferevent_socket_connect");
		return -1;
	}

	BaseLib::OS::sleep(1);
	int nSend = send(bufferevent_getfd(m_pBev), "test", 4, 0);
	if (nSend < 4)
	{
		DBGPRINT(DBG_ERROR, "connect false! send:" << nSend);
		return -1;
	}

	bufferevent_setcb(m_pBev, server_msg_cb, NULL, event_cb, this);
	bufferevent_enable(m_pBev, EV_READ | EV_WRITE);

	m_spWorker.reset(new CClientWorker());
	m_spWorker->SetBev(m_pBev);
	m_spListenThread->join();
	m_spListenThread.reset(new std::thread([]
	(void* arg)
	{
		auto me = (struct event_base *)arg;
		event_base_dispatch(me);
		DBGPRINT(DBG_ERROR, "event_base_dispatch break;");
	}, m_pBase));
	m_bConnect = TRUE;
	TryAgain();
	return TRUE;
}


BOOL LibEvtClient::SetIPAndPort(string& strIP, int nPort)
{
	if (m_bConnect)
	{
		return FALSE;
	}
	m_strIP = strIP;
	m_nPort = nPort;
	return TRUE;
}

int LibEvtClient::InitSystem(std::string & strIP, int nPort)
{
	if (IsConnect())
	{
		DBGPRINT(DBG_ERROR,"IsConnect");
		return 0;
	}

	SetIPAndPort(strIP, nPort);
#ifdef WIN32
	evthread_use_windows_threads();
	WinSockInit();
#else
	evthread_use_pthreads();
#endif // WIN32

	//两个参数依次是服务器端的IP地址、端口号
	m_pBase = event_base_new();

	m_pBev = bufferevent_socket_new(m_pBase, -1,
		 BEV_OPT_CLOSE_ON_FREE);

	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(nPort);
	//inet_aton(argv[1], &server_addr.sin_addr);
	inet_pton(AF_INET, strIP.c_str(), &server_addr.sin_addr);
	
	int nRt = -1;
	nRt = bufferevent_socket_connect(m_pBev, (struct sockaddr *)&server_addr,
		sizeof(server_addr));
	if (nRt == -1)
	{
		DBGPRINT(DBG_ERROR,"connect false! bufferevent_socket_connect");
		return -1;
	}

	BaseLib::OS::sleep(1);
	int nSend = send(bufferevent_getfd(m_pBev), "test", 4, 0);
	if (nSend < 4)
	{
		DBGPRINT(DBG_ERROR, "connect false! send:"<< nSend);
		return -1;
	}

	bufferevent_setcb(m_pBev, server_msg_cb, NULL, event_cb, this);
	bufferevent_enable(m_pBev, EV_READ |EV_WRITE);

	m_spWorker.reset(new CClientWorker());
	m_spWorker->SetBev(m_pBev);

	m_spListenThread.reset(new std::thread([]
	(void* arg)
	{
		auto me = (struct event_base *)arg;
		//  Wait for events to become active, and run their callbacks.
		//This is a more flexible version of event_base_dispatch().
		//event_base_loop(me,0);
		event_base_dispatch(me);
		DBGPRINT(DBG_ERROR, "event_base_dispatch break;");
	}, m_pBase));

#if 0
	char buffer[4096];
	bool isBreak = false;
	while (!isBreak) {
		std::cout << "Input your data to server(\'q\' or \"quit\" to exit)" << std::endl;
		std::cin >> buffer;
		if (strcmp("q", buffer) == 0 || strcmp("quit", buffer) == 0) {
			isBreak = true;
			break;

		}
		else if(strcmp("f", buffer) == 0)
		{
			Json::StreamWriterBuilder builder;
			builder.settings_["indentation"] = "";
			std::unique_ptr<Json::StreamWriter> writeInfo(builder.newStreamWriter());
			Json::Value root;
			root[FILE_CLASS] = "class_1";
			root[FILE_STUDENT] = "zhang0";
			root[FILE_COURES] = "lession1";
			root[FILE_PATH] = "H:\\workspace\\CMake\\mysql\\build\\Debug\\Movej.program";
			ostringstream oStr;
			writeInfo->write(root,&oStr);
			SendFileToServer(oStr.str());

		}
		std::cout << "Your input is " << buffer << std::endl;

		//struct bufferevent* bev = (struct bufferevent*)arg;

		////把终端的消息发送给服务器端
		//bufferevent_write(bev, msg, ret);
		std::string strMsg(buffer);
		SendToServer(strMsg);
		
		string strFileList;
		GetFileList("class1","zhangsan",strFileList);
		cout << "file list:"<<endl << strFileList << endl;
        BaseLib::OS::sleep(1);
        //UpperFile("class2", "life", "lesson6", "config.ini" , "c:\\config.ini","T120_10", 1);
        //BaseLib::OS::sleep(1);
		DownloadFile("c:\\config.ini", "lesson6", 1, 0);
		BaseLib::OS::sleep(1);
//		Json::StreamWriterBuilder builder;
//		builder.settings_["indentation"] = "";
//		std::unique_ptr<Json::StreamWriter> writeInfo(builder.newStreamWriter());
//		Json::Value root;
//		root[FILE_DESCRIBE][FILE_CLASS] = "class_1";
//		root[FILE_DESCRIBE][FILE_STUDENT] = "zhang0";
//		root[FILE_DESCRIBE][FILE_COURES] = "lession1";
//		root[FILE_DESCRIBE][FILE_PATH] = "H:\\workspace\\CMake\\mysql\\build\\Debug\\Movej.program";
//		ostringstream oStr;
//		writeInfo->write(root, &oStr);
//		SendFileToServer(oStr.str());
//        BaseLib::OS::sleep(1);
//		break;
	}
#endif // DEBUG
	m_bConnect = TRUE;
	TryAgain();
	return nRt;
}

int LibEvtClient::UpperFile(string strClass, string strStudent, string strCoures, string strFileName, string strFilePath, string strDeviceNum,int nType)
{
	if (!m_bConnect)
	{
		DBGPRINT(DBG_LibevtClient_SEND,"it's not connect");
		return -1;
	}
	Json::StreamWriterBuilder builder;
	builder.settings_["indentation"] = "";
	std::unique_ptr<Json::StreamWriter> writeInfo(builder.newStreamWriter());
	Json::Value root;
	root[FILE_DESCRIBE][FILE_CLASS] = strClass;
	root[FILE_DESCRIBE][FILE_STUDENT] = strStudent;
	root[FILE_DESCRIBE][FILE_COURES] = strCoures;
	root[FILE_DESCRIBE][FILE_NAME] = strFileName;
	root[FILE_DESCRIBE][FILE_PATH] = strFilePath;
	root[FILE_DESCRIBE][FILE_TYPE] = nType;
	root[FILE_DESCRIBE][DEVICE_NUMBER] = strDeviceNum;
	ostringstream oStr;
    writeInfo->write(root, &oStr);
	SendFileToServer(oStr.str());
}

int LibEvtClient::DownloadFile(string strPath, string strCoures, int nType, int nTemplate)
{

	if (!m_bConnect)
	{
		DBGPRINT(DBG_LibevtClient_SEND, "it's not connect");
		return -1;
	}
    string strFileName;
    int npos = strPath.find_last_of("\\");
    if(npos!=strPath.npos)
    {
        strFileName=strPath.substr(npos+1);
    }else
    {
        npos = strPath.find_last_of("/");
        if(npos!=strPath.npos)
        {
            strFileName=strPath.substr(npos+1);
        }
    }
	Json::StreamWriterBuilder builder;
	builder.settings_["indentation"] = "";
	std::unique_ptr<Json::StreamWriter> writeInfo(builder.newStreamWriter());
	Json::Value root;
    root[FILE_DESCRIBE][FILE_CLASS] = "Class";
    root[FILE_DESCRIBE][FILE_NAME] = strFileName;
	root[FILE_DESCRIBE][FILE_COURES] = strCoures;
	root[FILE_DESCRIBE][FILE_PATH] = strPath;
	root[FILE_DESCRIBE][FILE_TYPE] = nType;
	root[FILE_DESCRIBE][FILE_TEMPLATE] = nTemplate;
	ostringstream oStr;
	writeInfo->write(root, &oStr);

	m_spWorker->setCMDFinishFlag(-1);
	int nRt = m_spWorker->SendXZWJ(oStr.str());
	if(nRt<0)
		m_spWorker->waitCMDFinishFlag();
}

int LibEvtClient::GetFileList(string strClass, string strStudent, string& strOut)
{
	if (!m_bConnect)
	{
		DBGPRINT(DBG_LibevtClient_SEND, "it's not connect");
		return -1;
	}
	Json::StreamWriterBuilder builder;
	builder.settings_["indentation"] = "";
	std::unique_ptr<Json::StreamWriter> writeInfo(builder.newStreamWriter());
	Json::Value root;
	root[FILE_CLASS] = strClass;
	root[FILE_STUDENT] = strStudent;
	ostringstream oStr;
	writeInfo->write(root, &oStr);
	m_spWorker->setCMDFinishFlag(-1);
	int nRt = m_spWorker->SendWJLB(oStr.str());
	if(nRt<0)
		m_spWorker->waitCMDFinishFlag();
	strOut = m_spWorker->GetFileList();
}


string LibEvtClient::GetDeviceNum(string strIP)
{
	if (!m_bConnect)
	{
		DBGPRINT(DBG_LibevtClient_SEND, "it's not connect");
		return "not connect";
	}
    m_spWorker->setCMDFinishFlag(-1);
    m_spWorker->SendSBBH(strIP);
    m_spWorker->waitCMDFinishFlag();
    return m_spWorker->GetDeviceNumber();
}


string LibEvtClient::GetClass()
{
	if (!m_bConnect)
	{
		DBGPRINT(DBG_LibevtClient_SEND, "it's not connect");
		return "not connect";
	}
	return m_spWorker->GetClass();
}

string LibEvtClient::GetCourse()
{
	if (!m_bConnect)
	{
		DBGPRINT(DBG_LibevtClient_SEND, "it's not connect");
		return "not connect";
	}
	return m_spWorker->GetCourse();
}

string LibEvtClient::GetClassStudent()
{
	if (!m_bConnect)
	{
		DBGPRINT(DBG_LibevtClient_SEND, "it's not connect");
		return "not connect";
	}
	return m_spWorker->GetClassStudent();
}


void LibEvtClient::server_msg_cb(struct bufferevent* bev, void* arg)
{
	string strMsg;
	while (1)
	{

		int length = PAGSIZE + 100;
		char* msg = new char[length + 1];
		size_t len = bufferevent_read(bev, msg, sizeof(msg));
		if (len <= 0)
		{
			delete[] msg;
			break;
		}
		msg[len] = 0;
		strMsg += msg;
		delete[] msg;
	}

	//printf("recv %s from server\n", strMsg);
	LibEvtClient * plt = (LibEvtClient*)arg;
	plt->Process(strMsg);
}


void LibEvtClient::event_cb(struct bufferevent *bev, short event, void *arg)
{
	LibEvtClient* pClient = (LibEvtClient*)arg;
	if (event & BEV_EVENT_EOF)
	{
		DBGPRINT(DBG_ERROR, "connection closed");
	}
	else if (event & BEV_EVENT_ERROR)
	{
		DBGPRINT(DBG_ERROR, "some other error");
	}
	else if (event & BEV_EVENT_CONNECTED)
	{
		DBGPRINT(DBG_ERROR, "the client has connected to server\n");
		return;
	}

	//这将自动close套接字和free读写缓冲区
	//bufferevent_free(bev); 
	//event_base_loopbreak(pClient->m_pBase);
	pClient->SetConnect(FALSE);

}
void LibEvtClient::SendToServer(std::string& strMsg)
{
	//把消息发送给服务器端
	int fd = bufferevent_getfd(m_pBev);
	int len = send(fd, strMsg.c_str(), strMsg.length(), 0);
	//bufferevent_write(m_pBev, strMsg.c_str(), strMsg.length());
}

void LibEvtClient::Process(string strData)
{
	m_spWorker->Process(strData);
}

void LibEvtClient::SendFileToServer(string strData)
{
	m_spWorker->SendFileTo("CXWJ",strData);
}

string strHeart("h");
INT32 LibEvtClient::Svc(void) {
	BaseLib::OS::sleep(1);
	if (IsConnect())
	{
		int n = m_spWorker->SendMsgTo(strHeart);
		if (n < 0)
		{
			DBGPRINT(DBG_ERROR,"连接断开，尝试重连");
			TryConnect();	
		}
	}
	else
	{
		TryConnect();
		DBGPRINT(DBG_LibevtClient_SEND, "未连接，尝试重连");
	}
	
	return TASK_CONTINUE;
}
