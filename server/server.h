#pragma once

#include "../common/function.h"
#include "baselib.h"
#include <queue>
#include <mutex>
#include <vector>
#include <map>
#include <thread>
#include "work.h"
#define MUL_LIBEVENT_THREAD
#define THREAD_NUMB 4
using namespace std;
//多线程版Libevent
struct conn_queue_item {
	int fd;
	string strIP;
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


	std::mutex data_mtx;		//数据锁
	std::map<evutil_socket_t, CServerWorker>  data_map;		//存储读到的数据

	std::mutex conn_mtx;                        //维护连接队列的锁
	std::queue<conn_queue_item>  conn_queue;    //conn_queue 是一个管理conn_queue_item的队列
};
//void notify_cb(evutil_socket_t fd, short which, /*LibeventThread **/void *pLibeventThread);
//void conn_readcb(bufferevent *bev, void *arg);
//void conn_eventcb(bufferevent *bev, short events, void *arg);
class EXPORTDLL LibEvtServer
	:public BaseLib::ISingleton<LibEvtServer>
{
public:
	LibEvtServer();
	~LibEvtServer();

	int Give_A_Class(string strClass,string strCourse);
	int SendFile(string strPath, int nType);
	string GetClass();			//当前上课班级
	string GetCourse();			//当前课程
	string GetFile();			//下发文件
	int GetType();				//下发文件类型
	string GetConnectDevice();	//获取链接设备

	static void listener_cb(struct evconnlistener *listener, evutil_socket_t fd,
		struct sockaddr *sa, int socklen, void *user_data);
	static void notify_cb(evutil_socket_t fd, short which, void *pLibeventThread);
	static void conn_readcb(bufferevent *bev, void *arg);
	static void conn_eventcb(bufferevent *bev, short events, void *arg);
private:
	bool init();
	bool init_threads(int thread_numb);
	void setup_libevent_thread(LibeventThread * pLibeventThread);
	bool listen(int* port);
	
	struct event_base * m_base;
	int m_last_thread;	//最后一个线程
	std::vector<LibeventThread*> m_libevent_threads;
	struct evconnlistener * m_listener;
	std::shared_ptr<std::thread>   m_spListenThread;    //accept 线程

	string m_strClass;			//当前上课班级
	string m_strCourse;			//当前课程
	string m_strFilePath;		//一键下发文件路径
	int m_nType;				//下发文件类型

public:
	std::mutex m_device_mtx;
	std::vector<string> m_device_vec;	//在线设备
};

//int server();
//void listener_cb(evconnlistener *listener, evutil_socket_t fd,
//	struct sockaddr *sock, int socklen, void *arg);
//
//void socket_read_cb(bufferevent *bev, void *arg);
//void socket_event_cb(bufferevent *bev, short events, void *arg);
