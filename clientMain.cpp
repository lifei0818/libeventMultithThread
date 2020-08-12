#include <stdio.h>
#include "client/client.h"
#include "common/function.h"
int main()
{
	//evthread_use_pthreads();
	event_enable_debug_mode();
	//替换记录日志的默认函数
	event_enable_debug_logging(EVENT_DBG_ALL);
	event_set_log_callback(log_callback);
#ifdef WIN32
	WinSockInit();
#endif
	//server();
	char buffName[16] = { "client" };
    char buffip[16] = { "192.168.0.51" };
	char buffPort[16] = { "12345" };
	char* buff[3];// [64] = { "127.0.0.1","9999" };
	buff[0] = buffName;
	buff[1] = buffip;
	buff[2] = buffPort;
	char** argv = buff;
	//client(3, argv);
	//client2(3, argv);
	LibEvtClient libEvt;
	std::string strIp(argv[1]);
	int nPort = atoi(argv[2]);
	libEvt.InitSystem(strIp,nPort);
	
	//while (1) ;
	return 0;
}
