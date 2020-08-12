#pragma once
#include "../common/function.h"
#include "baselib.h"
#include<errno.h>
#include <thread>

int tcp_connect_server(const char* server_ip, int port);
int client(int argc, char** argv);
void cmd_msg_cb(evutil_socket_t fd, short events, void* arg);

void socket_read_cb(intptr_t fd, short events, void *arg);
int client2(int argc, char** argv);
class LibEvtClient {
public:
	LibEvtClient() {}
	~LibEvtClient() {}

	int InitSystem(std::string& strIP, int nPort);
private:
	static void server_msg_cb(struct bufferevent* bev, void* arg);
	static void event_cb(struct bufferevent *bev, short event, void *arg);
	void SendToServer(std::string& strMsg);
private:
	struct event_base * m_pBase;
	struct bufferevent* m_pBev;
	std::shared_ptr < std::thread > m_spListenThread;
};