#include "server.h"
server::server(int number)
	:m_nNumber(number)
	, m_last_thread(-1)
{
}

server::~server()
{
}
void server::start()
{
	Open(m_nNumber);
	server();
}
INT32 server::Svc(void)
{
	int index = 0;
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		index = (m_last_thread + 1) % m_nNumber;	//�̱߳�ţ���1��ʼ
	}

	//struct event_base * thread_base;            // �¼�����
	//struct event   notify_event;
	//evutil_socket_t  notfiy_recv_fd;            // socketpair ���ն�fd�������߳̽���֪ͨ��
	//evutil_socket_t  notfiy_send_fd;            // socketpair ���Ͷ�fd�������̷߳���֪ͨ��
	//std::mutex conn_mtx;                        //ά�����Ӷ��е���
	//std::queue<conn_queue_item>  conn_queue;    //conn_queue ��һ������conn_queue_item�Ķ���

	////�����߳��¼�notify_event
	//event_set(&notify_event, notfiy_recv_fd,//EV_READ��ʾֻҪ���socket�ɶ��͵���notify_cb����
	//	EV_READ | EV_PERSIST, ::notify_cb, plt);
	////�����¼���event_base�Ĺ�ϵ
	//event_base_set(plt->thread_base, &plt->notify_event); // �����¼��Ĵ�����ϵ���൱��ָ���¼������ĸ�event_base��
	////�����¼�
	//event_add(&plt->notify_event, 0); // ��ʽ�����¼�    
	return INT32();
}
///////////////////////////////////////////////////////////////////////////////////
int LibEvtServer::m_last_thread = -1;
std::vector<LibeventThread*> LibEvtServer::m_libevent_threads;

bool LibEvtServer::init(/*I_NetServerEvent* event, int start, int size*/)
{
	//m_ids = new ChannelIDGenerator();
	//m_ids->init(start, size);
	//m_allChannels.resize(m_ids->getSize());

	//m_event = event;


	////event֧��windows���̵߳ĺ���
	//int hr = evthread_use_windows_threads();
	m_base = event_base_new();
	if (!m_base) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return false;
	}
#ifdef MUL_LIBEVENT_THREAD
	m_last_thread = -1; //ע���ʼ��Ϊ-1
	//��ʼ���߳�
	init_threads(THREAD_NUMB);
#endif
	return true;
}

bool LibEvtServer::init_threads(int thread_numb)
{
	m_libevent_threads.resize(thread_numb);

	//Ϊÿ���߳�ָ��˫��ͨ���������ڹܵ���
	for (int i = 0; i < thread_numb; ++i)
	{

		LibeventThread* plt = new LibeventThread();
#ifdef WIN32
		//����һ��socketpair�����뻥��ͨ�ŵ�����socket��������fds����
		evutil_socket_t fds[2];
		if (evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds) < 0)
		{
			std::cout << "����socketpairʧ��\n";
			return false;
		}
		//���ó���������socket
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

		//��װlibevent�߳�[����base��ע��ͨ���¼������ڼ��������ӣ�]
		setup_libevent_thread(plt);

		//�̷߳���������
		m_libevent_threads[i] = plt;
	}

	//��ʼ�����������߳�
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

//�����߳���Ϣ
void LibEvtServer::setup_libevent_thread(LibeventThread * pLibeventThread)
{
	auto plt = pLibeventThread;
	plt->thread_base = event_base_new(); // �����̵߳�event_base

	//��ÿ��libevent�߳���������֪ͨ�ص�������
	plt->that = this;
	//�����߳��¼�notify_event
	event_set(&plt->notify_event, plt->notfiy_recv_fd,//EV_READ��ʾֻҪ���socket�ɶ��͵���notify_cb����
		EV_READ | EV_PERSIST, /*::notify_cb*/::notify_cb, plt);
	//�����¼���event_base�Ĺ�ϵ
	event_base_set(plt->thread_base, &plt->notify_event); // �����¼��Ĵ�����ϵ���൱��ָ���¼������ĸ�event_base��
	//�����¼�
	event_add(&plt->notify_event, 0); // ��ʽ�����¼�    
}


void LibEvtServer::listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
	struct sockaddr *sa, int socklen, void *user_data)
{
#ifdef MUL_LIBEVENT_THREAD
	int cur_thread = (LibEvtServer::m_last_thread + 1) % THREAD_NUMB; // ��ѭѡ�����߳�
	m_last_thread = cur_thread;

	conn_queue_item item;
	item.fd = fd;
	//item.ch2 = NULL;

	auto  plt = m_libevent_threads[cur_thread];
	{
		//���̵߳Ķ����з���һ��item��ÿ���߳��и����У��������ӵ�socketfd
#ifdef BOOST_LOCKFREE
		while (!plt->conn_queue.push(item))
		{
#ifndef _DEBUG
			boost::this_thread::interruptible_wait(1);
#else
			Sleep(1);
#endif
			Plug::PlugMessageBox("���Ӷ��о�Ȼ���ˣ�����1000��δ��������");
		}
#else
		std::lock_guard<std::mutex> lock(plt->conn_mtx);
		plt->conn_queue.push(item);
#endif
	}
	//������̵߳Ķ��¼�
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



//�����˿�,-1��ʾ��ϵͳ����һ��������ö˿�
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
	m_spListenThread.reset(new std::thread([this]//���ڿ�����߳�ֻ���յ����ӣ�Ȼ�󽻸��̣߳�Ȼ��֪ͨ�߳�
	{
		//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
		//event_base_loop(m_base, EVLOOP_ONCE);
		event_base_dispatch(m_base);
		if (WSAENOTSOCK == WSAGetLastError())
		{
			//Plug::PlugMessageBox(L"������Ч�׽��ְ���");
		}

		//Plug::PlugMessageBox(L"Libevent�ɷ��߳��˳���");
	}));
	return true;
}

void /*LibEvtServer::*/notify_cb(evutil_socket_t fd, short which, /*LibeventThread*/void *pLibeventThread)
{
	//���Ƚ�socketpair��1���ֽ�֪ͨ�źŶ��������Ǳ���ģ���ˮƽ����ģʽ��������������¼������ѭ��֪ͨ��ֱ���¼���������
	char  buf[1];
	recv(fd, buf, 1, 0);//��sockpair����һ�˶�����

	auto plt = (LibeventThread *)pLibeventThread;

	conn_queue_item  item;

	//���Լ������Ӷ�����ȡ��������
	{
		//ȡ�������еĵ�һ��Ԫ��
#ifdef BOOST_LOCKFREE
		while (!plt->conn_queue.pop(item))//popһ������
		{
#ifndef _DEBUG
			boost::this_thread::interruptible_wait(1);
#else
			Sleep(1);
#endif
			Plug::PlugMessageBox("֪ͨ���о�Ȼ�����˰���");
		}
#else
		std::lock_guard<std::mutex>  lck(plt->conn_mtx);
		item = plt->conn_queue.front();
#endif
	}

	//����ÿ��socket��bufferevent
	auto bev = bufferevent_socket_new(plt->thread_base, item.fd, BEV_OPT_THREADSAFE);

	//Channel2* c2 = CreateChannel(bev);

	//���ý��ա�״̬�ı� �ص�����
	bufferevent_setcb(bev, ::socket_read_cb, NULL, ::socket_event_cb, (void*)bev);
	bufferevent_enable(bev, EV_READ | EV_WRITE);
}
void /*LibEvtServer::*/conn_readcb(bufferevent *bev, void *arg)
{
	char msg[4096];

	size_t len = bufferevent_read(bev, msg, sizeof(msg) - 1);

	msg[len] = '\0';
	printf("server read the data %s\n", msg);

	char reply[] = "I has read your data";
	bufferevent_write(bev, reply, strlen(reply));
}
void /*LibEvtServer::*/conn_eventcb(bufferevent *bev, short events, void *arg)
{
	if (events & BEV_EVENT_EOF)
		printf("connection closed\n");
	else if (events & BEV_EVENT_ERROR)
		printf("some other error\n");

	//�⽫�Զ�close�׽��ֺ�free��д������  
	bufferevent_free(bev);
}

//����������̾���������ӵģ������߳̽��յ�����֮������socket����Libevent�̣߳�libevent����bufferevent
//������ض���д�¼����������ͨ��ÿ���̵߳����Ӷ��У�Ȼ��һ��socketpair֪ͨ�ġ�����ÿ���߳̾ͺ�ƽ���Ĵ������е������¼�
//���̱߳ȵ��̵߳��Ǹ��Ӻֻܶ࣬������ģʽ��֪������bufferevent����һ����
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define listen_number 20
int server()
{
#if 1

	//evthread_use_pthreads();//enable threads  
	//evthread_use_windows_threads();

#ifdef EVTHREAD_USE_WINDOWS_THREADS_IMPLEMENTED
	evthread_use_windows_threads();
#endif
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(struct sockaddr_in));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(9999);

	event_base *base = event_base_new();
	evconnlistener *listener
		= evconnlistener_new_bind(base, listener_cb, base,
			LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
			listen_number, (struct sockaddr*)&sin,
			sizeof(struct sockaddr_in));

	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_base_free(base);
#endif
	return 0;
}


//һ���¿ͻ��������Ϸ�������  
//���˺���������ʱ��libevent�Ѿ�������accept������ͻ��ˡ��ÿͻ��˵�
//�ļ�������Ϊfd
void listener_cb(evconnlistener *listener, evutil_socket_t fd,
	struct sockaddr *sock, int socklen, void *arg)
{
	printf("accept a client %d\n", fd);

	event_base *base = (event_base*)arg;

	//Ϊ����ͻ��˷���һ��bufferevent  
	bufferevent *bev = bufferevent_socket_new(base, fd,
		BEV_OPT_CLOSE_ON_FREE);

	bufferevent_setcb(bev, socket_read_cb, NULL, socket_event_cb, (void*)bev);
	bufferevent_enable(bev, EV_READ | EV_PERSIST);
}

void socket_read_cb(bufferevent *bev, void *arg)
{
	char msg[4096];

	size_t len = bufferevent_read(bev, msg, sizeof(msg) - 1);

	msg[len] = '\0';
	printf("server read the data %s\n", msg);

	char reply[] = "I has read your data";
	bufferevent_write(bev, reply, strlen(reply));
}

void socket_event_cb(bufferevent *bev, short events, void *arg)
{
	if (events & BEV_EVENT_EOF)
		printf("connection closed\n");
	else if (events & BEV_EVENT_ERROR)
		printf("some other error\n");

	//�⽫�Զ�close�׽��ֺ�free��д������  
	bufferevent_free(bev);
}