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

int LibEvtClient::InitSystem(std::string & strIP, int nPort)
{

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

	int nRt = bufferevent_socket_connect(m_pBev, (struct sockaddr *)&server_addr,
		sizeof(server_addr));
	if (nRt == -1)
	{
		DBGPRINT(DBG_ERROR,"connect false!");
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
	}, m_pBase));

#if 1
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
	return 0;
}

void LibEvtClient::UpperFile(string strClass, string strStudent, string strCoures, string strFileName, string strFilePath, string strDeviceNum,int nType)
{

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
    BaseLib::OS::sleep(1);
}

void LibEvtClient::DownloadFile(string strPath, string strCoures, int nType, int nTemplate)
{
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
	m_spWorker->SendXZWJ(oStr.str());
	m_spWorker->waitCMDFinishFlag();
}

void LibEvtClient::GetFileList(string strClass, string strStudent, string& strOut)
{
	Json::StreamWriterBuilder builder;
	builder.settings_["indentation"] = "";
	std::unique_ptr<Json::StreamWriter> writeInfo(builder.newStreamWriter());
	Json::Value root;
	root[FILE_CLASS] = strClass;
	root[FILE_STUDENT] = strStudent;
	ostringstream oStr;
	writeInfo->write(root, &oStr);
	m_spWorker->setCMDFinishFlag(-1);
	m_spWorker->SendWJLB(oStr.str());
	m_spWorker->waitCMDFinishFlag();
	strOut = m_spWorker->GetFileList();
}


string LibEvtClient::GetDeviceNum(string strIP)
{
    m_spWorker->setCMDFinishFlag(-1);
    m_spWorker->SendSBBH(strIP);
    m_spWorker->waitCMDFinishFlag();
    return m_spWorker->GetDeviceNumber();
}

void LibEvtClient::server_msg_cb(struct bufferevent* bev, void* arg)
{
	char msg[1024];

	size_t len = bufferevent_read(bev, msg, sizeof(msg));
	msg[len] = '\0';

	printf("recv %s from server\n", msg);
	LibEvtClient * plt = (LibEvtClient*)arg;
	plt->Process(msg);
}


void LibEvtClient::event_cb(struct bufferevent *bev, short event, void *arg)
{

	if (event & BEV_EVENT_EOF)
		printf("connection closed\n");
	else if (event & BEV_EVENT_ERROR)
		printf("some other error\n");
	else if (event & BEV_EVENT_CONNECTED)
	{
		printf("the client has connected to server\n");
		return;
	}

	//这将自动close套接字和free读写缓冲区
	bufferevent_free(bev);
}
void LibEvtClient::SendToServer(std::string& strMsg)
{
	//把消息发送给服务器端
	int fd = bufferevent_getfd(m_pBev);
	int len = send(fd, strMsg.c_str(), strMsg.length(), 0);
	std::cout << "send length:" << len << std::endl;
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
