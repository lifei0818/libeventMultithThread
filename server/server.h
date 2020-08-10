#pragma once

#include "../common/function.h"
#include "baselib.h"
#include <queue>
#include <mutex>
#include <vector>
#define MUL_LIBEVENT_THREAD
#define THREAD_NUMB 10
class server
	: public BaseLib::CTask
{
public:
	server(int number=4);
	~server();
	void  start();		//accept
	INT32 Svc(void) ;	//客户端服务线程
private:
	int m_last_thread;	//最后一个线程
	int m_nNumber;		//线程个数
	//std::queue<int> m_queue;
	std::mutex  m_mutex;
};

//多线程版Libevent
struct conn_queue_item {
	int fd;
};

class LibEvtServer;
//保存线程的结构体
struct LibeventThread
{
	LibEvtServer* that;                            //用作传参
	std::shared_ptr<std::thread>   spThread;    // 线程
	struct event_base * thread_base;            // 事件根基
	struct event   notify_event;
	evutil_socket_t  notfiy_recv_fd;            // socketpair 接收端fd（工作线程接收通知）
	evutil_socket_t  notfiy_send_fd;            // socketpair 发送端fd（监听线程发送通知）
#ifdef BOOST_LOCKFREE
	boost::lockfree::spsc_queue<conn_queue_item, boost::lockfree::capacity<1000> > conn_queue;
#else
	std::mutex conn_mtx;                        //维护连接队列的锁
	std::queue<conn_queue_item>  conn_queue;    //conn_queue 是一个管理conn_queue_item的队列
#endif
};
void notify_cb(evutil_socket_t fd, short which, /*LibeventThread **/void *pLibeventThread);
void conn_readcb(bufferevent *bev, void *arg);
void conn_eventcb(bufferevent *bev, short events, void *arg);
class LibEvtServer
{
public:
	static int m_last_thread;	//最后一个线程
	static std::vector<LibeventThread*> m_libevent_threads;
	bool init(/*I_NetServerEvent* event, int start, int size*/);
	bool init_threads(int thread_numb);
	void setup_libevent_thread(LibeventThread * pLibeventThread);
	static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
		struct sockaddr *sa, int socklen, void *user_data);
	bool listen(int* port);

	//void notify_cb(evutil_socket_t fd, short which, LibeventThread *pLibeventThread);
	//static void conn_readcb(bufferevent *bev, void *arg);
	//static void conn_eventcb(bufferevent *bev, short events, void *arg);
	struct event_base * m_base;
private:
	struct evconnlistener * m_listener;
	std::shared_ptr<std::thread>   m_spListenThread;    //accept 线程
};

#include<stdio.h>  
#include<string.h>  

#include<event.h>  
#include<event2/listener.h>  
#include<event2/bufferevent.h>  

int server();
void listener_cb(evconnlistener *listener, evutil_socket_t fd,
	struct sockaddr *sock, int socklen, void *arg);

void socket_read_cb(bufferevent *bev, void *arg);
void socket_event_cb(bufferevent *bev, short events, void *arg);
