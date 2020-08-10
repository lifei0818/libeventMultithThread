#pragma once
class client
{
public:
	client();
	~client();
};

#include "../common/function.h"
#include "baselib.h"
#include<sys/types.h>
//#include<sys/socket.h>
//#include<netinet/in.h>
//#include<arpa/inet.h>
#include<errno.h>
//#include<unistd.h>

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#include<event.h>
#include<event2/bufferevent.h>
#include<event2/buffer.h>
#include<event2/util.h>
#include <thread>

int tcp_connect_server(const char* server_ip, int port);
int client(int argc, char** argv);
void cmd_msg_cb(evutil_socket_t fd, short events, void* arg);
void server_msg_cb(struct bufferevent* bev, void* arg);
void event_cb(struct bufferevent *bev, short event, void *arg);


void socket_read_cb(intptr_t fd, short events, void *arg);
int client2(int argc, char** argv);
