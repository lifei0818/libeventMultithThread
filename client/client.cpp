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

	bufferevent_socket_connect(m_pBev, (struct sockaddr *)&server_addr,
		sizeof(server_addr));


	bufferevent_setcb(m_pBev, server_msg_cb, NULL, event_cb, NULL);
	bufferevent_enable(m_pBev, EV_READ |EV_WRITE);

	m_spListenThread.reset(new std::thread([]
	(void* arg)
	{
		auto me = (struct event_base *)arg;
		//  Wait for events to become active, and run their callbacks.
		//This is a more flexible version of event_base_dispatch().
		//event_base_loop(me,0);
		event_base_dispatch(me);
	}, m_pBase));


	char buffer[4096];
	bool isBreak = false;
	while (!isBreak) {
		std::cout << "Input your data to server(\'q\' or \"quit\" to exit)" << std::endl;
		std::cin >> buffer;
		if (strcmp("q", buffer) == 0 || strcmp("quit", buffer) == 0) {
			isBreak = true;
			break;

		}
		std::cout << "Your input is " << buffer << std::endl;

		//struct bufferevent* bev = (struct bufferevent*)arg;

		////把终端的消息发送给服务器端
		//bufferevent_write(bev, msg, ret);
		std::string strMsg(buffer);
		SendToServer(strMsg);
		
		//int write_num = write(sockfd, buffer, strlen(buffer));
		//std::cout << write_num << " characters written" << std::endl;
		Sleep(2000);
	}
	return 0;
}

void LibEvtClient::server_msg_cb(struct bufferevent* bev, void* arg)
{
	char msg[1024];

	size_t len = bufferevent_read(bev, msg, sizeof(msg));
	msg[len] = '\0';

	printf("recv %s from server\n", msg);
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

	struct event *ev = (struct event*)arg;
	event_free(ev);
}
void LibEvtClient::SendToServer(std::string& strMsg)
{
	//把消息发送给服务器端
	int fd = bufferevent_getfd(m_pBev);
	int len = send(fd, strMsg.c_str(), strMsg.length(), 0);
	std::cout << "send length:" << len << std::endl;
	//bufferevent_write(m_pBev, strMsg.c_str(), strMsg.length());
}