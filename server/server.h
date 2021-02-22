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
//���̰߳�Libevent
struct conn_queue_item {
	int fd;
	string strIP;
};

class LibEvtServer;
//�����̵߳Ľṹ��
struct LibeventThread
{
	LibEvtServer* that;                            //��������
	std::shared_ptr<std::thread>   spThread;    // �߳�
	struct event_base * thread_base;            // �¼�����
	struct event   notify_event;
	evutil_socket_t  notfiy_recv_fd;            // socketpair ���ն�fd�������߳̽���֪ͨ��
	evutil_socket_t  notfiy_send_fd;            // socketpair ���Ͷ�fd�������̷߳���֪ͨ��


	std::mutex data_mtx;		//������
	std::map<evutil_socket_t, CServerWorker>  data_map;		//�洢����������

	std::mutex conn_mtx;                        //ά�����Ӷ��е���
	std::queue<conn_queue_item>  conn_queue;    //conn_queue ��һ������conn_queue_item�Ķ���
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
	string GetClass();			//��ǰ�Ͽΰ༶
	string GetCourse();			//��ǰ�γ�
	string GetFile();			//�·��ļ�
	int GetType();				//�·��ļ�����
	string GetConnectDevice();	//��ȡ�����豸

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
	int m_last_thread;	//���һ���߳�
	std::vector<LibeventThread*> m_libevent_threads;
	struct evconnlistener * m_listener;
	std::shared_ptr<std::thread>   m_spListenThread;    //accept �߳�

	string m_strClass;			//��ǰ�Ͽΰ༶
	string m_strCourse;			//��ǰ�γ�
	string m_strFilePath;		//һ���·��ļ�·��
	int m_nType;				//�·��ļ�����

public:
	std::mutex m_device_mtx;
	std::vector<string> m_device_vec;	//�����豸
};

//int server();
//void listener_cb(evconnlistener *listener, evutil_socket_t fd,
//	struct sockaddr *sock, int socklen, void *arg);
//
//void socket_read_cb(bufferevent *bev, void *arg);
//void socket_event_cb(bufferevent *bev, short events, void *arg);
