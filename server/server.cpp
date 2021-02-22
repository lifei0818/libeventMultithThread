#include "server.h"

//int LibEvtServer::m_last_thread = -1;
//std::vector<LibeventThread*> LibEvtServer::m_libevent_threads;

LibEvtServer::LibEvtServer()
	:m_nType(0)
{
	init();
}

LibEvtServer::~LibEvtServer()
{
	auto iter = m_libevent_threads.begin();
	for (; iter != m_libevent_threads.end(); iter++)
	{
		event_base_loopbreak((*iter)->thread_base);
		//BaseLib::OS::sleep(1);
		event_base_free((*iter)->thread_base);
		(*iter)->spThread->join();
		delete (*iter);
	}
	evconnlistener_free(m_listener);
	event_base_loopbreak(m_base);
	event_base_free(m_base);
	m_spListenThread->join();
}


int LibEvtServer::Give_A_Class(string strClass, string strCourse)
{
	m_strClass = strClass;
	m_strCourse = strCourse;

	int nRt = 0;
	auto iter = m_libevent_threads.begin();
	for (;iter!= m_libevent_threads.end(); iter++)
	{
		send((*iter)->notfiy_send_fd, "a", 1, 0);
	}
	return nRt;
}

int LibEvtServer::SendFile(string strPath,int nType)
{
	m_nType = nType;
	m_strFilePath=strPath; 
	
	int nRt = 0;
	auto iter = m_libevent_threads.begin();
	for (; iter != m_libevent_threads.end(); iter++)
	{
		send((*iter)->notfiy_send_fd, "b", 1, 0);
	}
	return nRt;
}


string LibEvtServer::GetClass() {
	return m_strClass;
}


string LibEvtServer::GetCourse()
{
	return m_strCourse;
}

string LibEvtServer::GetFile()
{
	return m_strFilePath;
}

int LibEvtServer::GetType()
{
	return m_nType;
}

string LibEvtServer::GetConnectDevice() {
	string strRt;
	string strHert("H");
	int nSize = m_libevent_threads.size();
	for (int i = 0; i < nSize; i++) {
		m_libevent_threads[i]->data_mtx;
		auto iter = m_libevent_threads[i]->data_map.begin();
		for (; iter != m_libevent_threads[i]->data_map.end(); iter++)
		{
			strRt += iter->second.GetIP()+ DECOLLATOR;
			//发送心跳
			iter->second.SendMsgTo(strHert);
		}
	}
	return strRt;
}

bool LibEvtServer::init()
{
	BaseLib::InitBaseLib();
	INIT_DBG_CODE
	////event支持windows下线程的函数
#ifdef WIN32
	evthread_use_windows_threads();
	WinSockInit();
#else
	evthread_use_pthreads();
#endif // WIN32

	//替换记录日志的默认函数
	event_enable_debug_mode();
	event_enable_debug_logging(EVENT_DBG_ALL);
	event_set_log_callback(log_callback);

	m_base = event_base_new();
	if (!m_base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return false;
	}
#ifdef MUL_LIBEVENT_THREAD
	m_last_thread = -1; //注意初始化为-1
	//初始化线程
	init_threads(THREAD_NUMB);
#endif
	string strPath = BaseLib::GetModuleFullPath(true) + PATH_SEP_STRING;
	BaseLib::CIniFile iniFile((strPath + "config.ini").c_str());
	int port = iniFile.ReadInteger("Server", "port", 8888);
	listen(&port);
	return true;
}

bool LibEvtServer::init_threads(int thread_numb)
{
	m_libevent_threads.resize(thread_numb);

	//为每个线程指定双向通道（类似于管道）
	for (int i = 0; i < thread_numb; ++i)
	{

		LibeventThread* plt = new LibeventThread();
#ifdef WIN32
		//创建一个socketpair即可与互相通信的两个socket，保存在fds里面
		evutil_socket_t fds[2];
		if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds) < 0)
		{
			std::cout << "创建socketpair失败\n";
			return false;
		}
		//设置成无阻赛的socket
		evutil_make_socket_nonblocking(fds[0]);
		evutil_make_socket_nonblocking(fds[1]);
#else
		int fds[2];
		if (pipe(fds)) {
			perror("Can't create notify pipe");
			exit(1);
		}
#endif
		plt->notfiy_recv_fd = fds[0];
		plt->notfiy_send_fd = fds[1];

		//安装libevent线程[创建base，注册通道事件（用于监听新链接）]
		setup_libevent_thread(plt);

		//线程放入容器中
		m_libevent_threads[i] = plt;
	}

	//开始创建并启动线程
	for (int i = 0; i < thread_numb; ++i)
	{
		m_libevent_threads[i]->spThread.reset(new std::thread([]
		(void* arg)
		{
			auto me = (LibeventThread*)arg;
			//  Wait for events to become active, and run their callbacks.
			//This is a more flexible version of event_base_dispatch().
			event_base_loop(me->thread_base, 0);
		}, m_libevent_threads[i]));
	}
	return true;
}

//设置线程信息
void LibEvtServer::setup_libevent_thread(LibeventThread * pLibeventThread)
{
	auto plt = pLibeventThread;
	plt->thread_base = event_base_new(); // 创建线程的event_base

	//给每个libevent线程设置连接通知回调函数。
	plt->that = this;
	//设置线程事件notify_event
	event_set(&plt->notify_event, plt->notfiy_recv_fd,//EV_READ表示只要这个socket可读就调用notify_cb函数
		EV_READ | EV_PERSIST, /*::notify_cb*/plt->that->notify_cb, plt);
	//设置事件和event_base的关系
	event_base_set(plt->thread_base, &plt->notify_event); // 设置事件的从属关系（相当于指明事件属于哪个event_base）
	//添加事件
	event_add(&plt->notify_event, 0); // 正式添加事件    
}


void LibEvtServer::listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
	struct sockaddr *sa, int socklen, void *user_data)
{
	LibEvtServer* ptrServer = (LibEvtServer*)user_data;

	// 将sockaddr强制转换为 sockaddr_in
	sockaddr_in sin;
	memcpy(&sin, sa, sizeof(sin));
	// 取得ip和端口号
	char buff[64];
	sprintf(buff, "%s",inet_ntoa(sin.sin_addr));
	
#ifdef MUL_LIBEVENT_THREAD
	int cur_thread = (ptrServer->m_last_thread + 1) % THREAD_NUMB; // 轮循选择工作线程
	ptrServer->m_last_thread = cur_thread;

	conn_queue_item item;
	item.fd = fd;
	item.strIP = string(buff);

	auto  plt = ptrServer->m_libevent_threads[cur_thread];
	{
		//向线程的队列中放入一个item，每个线程有个队列，保存连接的socketfd
#ifdef BOOST_LOCKFREE
		while (!plt->conn_queue.push(item))
		{
#ifndef _DEBUG
			boost::this_thread::interruptible_wait(1);
#else
			Sleep(1);
#endif
		}
#else
		std::lock_guard<std::mutex> lock(plt->conn_mtx);
		plt->conn_queue.push(item);
#endif
	}
	//激活读线程的读事件
	send(plt->notfiy_send_fd, "c", 1, 0);

#else
	auto base = evconnlistener_get_base(listener);

	auto bev = bufferevent_socket_new(base, fd, BEV_OPT_THREADSAFE);//|BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		fprintf(stderr, "Error constructing bufferevent!");
		event_base_loopbreak(base);
		return;
	}

	auto c2 = CreateChannel(bev);

	bufferevent_setcb(bev, conn_readcb, NULL, conn_eventcb, c2);
	bufferevent_enable(bev, EV_READ | EV_WRITE);

#endif
}


//侦听端口,-1表示向系统申请一个任意可用端口
bool LibEvtServer::listen(int* port)
{
	struct sockaddr_in sin;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	if (-1 == *port)
		sin.sin_port = htons(10000);
	else
		sin.sin_port = htons(*port);

	m_listener = evconnlistener_new_bind(m_base, listener_cb, (void*)this,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
		(struct sockaddr*)&sin,
		sizeof(sin));
	if (!m_listener)
	{
		return false;
	}

	if (-1 == *port)
		*port = ntohs(sin.sin_port);

	if (!m_listener) {
		fprintf(stderr, "Could not create a listener!\n");
		return false;
	}
	m_spListenThread.reset(new std::thread([this]//现在看这个线程只是收到连接，然后交给线程，然后通知线程
	{
		event_base_dispatch(m_base);
	}));
	return true;
}

void LibEvtServer::notify_cb(evutil_socket_t fd, short which, void *pLibeventThread)
{
	auto plt = (LibeventThread *)pLibeventThread;

	//首先将socketpair的1个字节通知信号读出（这是必须的，在水平触发模式下如果不处理该事件，则会循环通知，直到事件被处理）
	char  buf[1];
	recv(fd, buf, 1, 0);//从sockpair的另一端读数据
	do {
		std::lock_guard<std::mutex> lock(plt->data_mtx);
		if (buf[0] == 'a')
		{
			string strClass = plt->that->GetClass();
			string strCourse = plt->that->GetCourse();
			auto iter = plt->data_map.begin();
			for (; iter != plt->data_map.end(); iter++)
			{
				iter->second.SendSKBJ(strClass, strCourse);
			}
			DBGPRINT(DBG_LibevtServer_SEND,"class:"<< strClass<<"\tcourse:"<< strCourse);
		}
		else if (buf[0] == 'b')
		{
			string strFile = plt->that->GetFile();
			string strFileName;
			int npos = strFile.rfind(PATH_SEPARATOR);
			if (npos != strFile.npos)
			{
				strFileName = strFile.substr(npos+1);
			}
			string strMsg;
			Json::Value root;
			root[FILE_DESCRIBE][FILE_CLASS] = plt->that->GetClass();
			root[FILE_DESCRIBE][FILE_NAME] = strFileName;
			root[FILE_DESCRIBE][FILE_COURES] = plt->that->GetCourse();;
			root[FILE_DESCRIBE][FILE_PATH] = plt->that->GetFile();
			root[FILE_DESCRIBE][FILE_TYPE] = plt->that->GetType();
			root[FILE_DESCRIBE][FILE_TEMPLATE] = 1;
			strMsg = CWorker::JsonToString(root);
			DBGPRINT(DBG_LibevtServer_SEND,"msg:"<<strMsg<<"\nstrPath:"<< strFile);
			auto iter = plt->data_map.begin();
			for (; iter != plt->data_map.end(); iter++)
			{
				iter->second.SendFileTo("CXWJ",strMsg);
				DBGPRINT(DBG_LibevtServer_SEND,"发送文件成功:"<<iter->second.GetIP());
			}
		}
		else
		{
			break;
		}
		return;
	} while (false);
	


	conn_queue_item  item;

	//从自己的连接队列中取出连接数
	{
		//取出队列中的第一个元素
		std::lock_guard<std::mutex>  lck(plt->conn_mtx);
		item = plt->conn_queue.front();
		plt->conn_queue.pop();
	}

	//创建每个socket的bufferevent
	auto bev = bufferevent_socket_new(plt->thread_base, item.fd, BEV_OPT_THREADSAFE|BEV_OPT_CLOSE_ON_FREE); //

	//为每个socket添加数据处理函数
	plt->data_map[item.fd].SetBev(bev);
	plt->data_map[item.fd].SetIP(item.strIP);
	plt->data_map[item.fd].SendSKBJ(plt->that->GetClass(), plt->that->GetCourse());

	//设置接收、状态改变 回调函数
	bufferevent_setcb(bev, plt->that->conn_readcb, NULL, plt->that->conn_eventcb, (void*)plt);
	bufferevent_enable(bev, EV_READ | EV_WRITE/*|EV_PERSIST*/);
}
void LibEvtServer::conn_readcb(bufferevent *bev, void *arg)
{
	string strMsg;
	while (1)
	{
		int length = PAGSIZE + 100;
		char* msg = new char [length + 1];
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
	
	auto plt = (LibeventThread *)arg;
	plt->data_map[bufferevent_getfd(bev)].Process(strMsg);
}
void LibEvtServer::conn_eventcb(bufferevent *bev, short events, void *arg)
{
	auto plt = (LibeventThread *)arg;
	if (events & BEV_EVENT_EOF)
		printf("connection closed\n");
	else if (events & BEV_EVENT_ERROR)
		printf("some other error\n");

	//这将自动close套接字和free读写缓冲区 
	std::lock_guard<std::mutex> lock(plt->data_mtx);
	auto iter = plt->data_map.find(bufferevent_getfd(bev));
	if (iter != plt->data_map.end())
	{
		plt->data_map.erase(iter);
	}
	bufferevent_free(bev);

	//std:lock_guard<std::mutex> lock(plt->that->m_device_mtx);
	//ptrServer->m_device_vec.push_back(string(buff));
}

//看了这个过程就是这个样子的，监听线程接收到连接之后把这个socket丢给Libevent线程，libevent创建bufferevent
//处理相关读和写事件，这个工程通过每个线程的连接队列，然后一个socketpair通知的。这样每个线程就很平均的处理所有的连接事件
//多线程比单线程的是复杂很多，只是这种模式不知道，但bufferevent还是一样的
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#define listen_number 20
//int server()
//{
//#if 1
//
//	//evthread_use_pthreads();//enable threads  
//	//evthread_use_windows_threads();
//
//#ifdef EVTHREAD_USE_WINDOWS_THREADS_IMPLEMENTED
//	evthread_use_windows_threads();
//#endif
//	struct sockaddr_in sin;
//	memset(&sin, 0, sizeof(struct sockaddr_in));
//	sin.sin_family = AF_INET;
//	sin.sin_port = htons(9999);
//
//	event_base *base = event_base_new();
//	evconnlistener *listener
//		= evconnlistener_new_bind(base, listener_cb, base,
//			LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
//			listen_number, (struct sockaddr*)&sin,
//			sizeof(struct sockaddr_in));
//
//	event_base_dispatch(base);
//
//	evconnlistener_free(listener);
//	event_base_free(base);
//#endif
//	return 0;
//}


//一个新客户端连接上服务器了  
//当此函数被调用时，libevent已经帮我们accept了这个客户端。该客户端的
//文件描述符为fd
//void listener_cb(evconnlistener *listener, evutil_socket_t fd,
//	struct sockaddr *sock, int socklen, void *arg)
//{
//	printf("accept a client %d\n", fd);
//
//	event_base *base = (event_base*)arg;
//
//	//为这个客户端分配一个bufferevent  
//	bufferevent *bev = bufferevent_socket_new(base, fd,
//		BEV_OPT_CLOSE_ON_FREE);
//
//	bufferevent_setcb(bev, socket_read_cb, NULL, socket_event_cb, (void*)bev);
//	bufferevent_enable(bev, EV_READ | EV_PERSIST);
//}
//
//void socket_read_cb(bufferevent *bev, void *arg)
//{
//	char msg[4096];
//
//	size_t len = bufferevent_read(bev, msg, sizeof(msg) - 1);
//
//	msg[len] = '\0';
//	printf("server read the data %s\n", msg);
//
//	char reply[] = "I has read your data";
//	bufferevent_write(bev, reply, strlen(reply));
//}

//void socket_event_cb(bufferevent *bev, short events, void *arg)
//{
//	if (events & BEV_EVENT_EOF)
//		printf("connection closed\n");
//	else if (events & BEV_EVENT_ERROR)
//		printf("some other error\n");
//
//	//这将自动close套接字和free读写缓冲区  
//	bufferevent_free(bev);
//}