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


int client(int argc, char** argv)
{
	if (argc < 3)
	{
		//两个参数依次是服务器端的IP地址、端口号
		printf("please input 2 parameter\n");
		return -1;
	}

	struct event_base *base = event_base_new();

	struct bufferevent* bev = bufferevent_socket_new(base, -1,
		BEV_OPT_CLOSE_ON_FREE);

	//监听终端输入事件
	//GetStdHandle(STD_INPUT_HANDLE);
	struct event* ev_cmd = event_new(base, 0,
		EV_READ | EV_PERSIST,
		cmd_msg_cb, (void*)bev);


	event_add(ev_cmd, NULL);

	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	//inet_aton(argv[1], &server_addr.sin_addr);
	inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

	bufferevent_socket_connect(bev, (struct sockaddr *)&server_addr,
		sizeof(server_addr));


	bufferevent_setcb(bev, server_msg_cb, NULL, event_cb, (void*)ev_cmd);
	bufferevent_enable(bev, EV_READ | EV_PERSIST);



	event_base_dispatch(base);

	printf("finished \n");
	return 0;
}


void cmd_msg_cb(evutil_socket_t fd, short events, void* arg)
{
	char msg[1024];

	//int ret = read(fd, msg, sizeof(msg));
	//int ret = read(fd, msg, sizeof(msg));
	int ret = recv(fd, msg, sizeof(msg), 0);
	if (ret < 0)
	{
		perror("read fail ");
		exit(1);
	}

	struct bufferevent* bev = (struct bufferevent*)arg;

	//把终端的消息发送给服务器端
	bufferevent_write(bev, msg, ret);
}


void server_msg_cb(struct bufferevent* bev, void* arg)
{
	char msg[1024];

	size_t len = bufferevent_read(bev, msg, sizeof(msg));
	msg[len] = '\0';

	printf("recv %s from server\n", msg);
}


void event_cb(struct bufferevent *bev, short event, void *arg)
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

	struct event *ev = (struct event*)arg;
	event_free(ev);
}

//############################################################################
void socket_read_cb(intptr_t fd, short events, void *arg)
{
	char msg[1024];

	//为了简单起见，不考虑读一半数据的情况
	//int len = read(fd, msg, sizeof(msg) - 1);
	int len = recv(fd, msg, sizeof(msg), 0);

	if (len <= 0)
	{
		perror("read fail ");
		exit(1);
	}

	msg[len] = '\0';

	printf("recv %s from server\n", msg);
}

typedef struct sockaddr SA;
int tcp_connect_server(const char* server_ip, int port)
{
	int sockfd, status, save_errno;
	struct sockaddr_in server_addr;

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	//status = inet_pton(server_ip, &server_addr.sin_addr);
	status = inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

	if (status == 0) //the server_ip is not valid value
	{
		errno = EINVAL;
		return -1;
	}

	sockfd = ::socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		return sockfd;


	status = ::connect(sockfd, (SA*)&server_addr, sizeof(server_addr));

	if (status == -1)
	{
		save_errno = errno;
		//::close(sockfd);
		errno = save_errno; //the close may be error
		return -1;
	}

	evutil_make_socket_nonblocking(sockfd);

	return sockfd;
}
#include <string>
int client2(int argc, char** argv)
{
	if (argc < 3)
	{
		printf("please input 2 parameter\n");
		return -1;
	}


	//两个参数依次是服务器端的IP地址、端口号
	int sockfd = tcp_connect_server(argv[1], atoi(argv[2]));
	if (sockfd == -1)
	{
		perror("tcp_connect error ");
		return -1;
	}

	printf("connect to server successful\n");

	struct event_base* base = event_base_new();

	struct event *ev_sockfd = event_new(base, sockfd,
		EV_READ | EV_PERSIST,
		socket_read_cb, NULL);
	event_add(ev_sockfd, NULL);

	//监听终端输入事件
	struct event* ev_cmd = event_new(base, 1,
		EV_READ | EV_PERSIST, cmd_msg_cb,
		(void*)&sockfd);


	//event_add(ev_cmd, NULL);
#if 0
	event_base_dispatch(base);
#else
	std::shared_ptr < std::thread > m_spListenThread;
	m_spListenThread.reset(new std::thread([]
	(void* arg)
	{
		auto me = (struct event_base *)arg;
		//  Wait for events to become active, and run their callbacks.
		//This is a more flexible version of event_base_dispatch().
		event_base_loop(me, 0);
	}, base));

	char buffer[4096];
	bool isBreak = false;
	while (!isBreak) {
		std::cout << "Input your data to server(\'q\' or \"quit\" to exit)" << std::endl;
		std::cin >> buffer;
		if (strcmp("q", buffer) == 0 || strcmp("quit", buffer) == 0) {
			isBreak = true;
			close(sockfd);
			break;

		}
		std::cout << "Your input is " << buffer << std::endl;

		//struct bufferevent* bev = (struct bufferevent*)arg;

		////把终端的消息发送给服务器端
		//bufferevent_write(bev, msg, ret);
		int write_num = 0;
		std::string str(buffer);
		if (write_num=send(sockfd, str.c_str(), str.length(), 0) < 0)
		{

		}
		//int write_num = write(sockfd, buffer, strlen(buffer));
		std::cout << write_num << " characters written" << std::endl;
		Sleep(2000);
	}
#endif
	printf("finished \n");
	return 0;
}
