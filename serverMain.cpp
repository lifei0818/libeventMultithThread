#include "common/function.h"
#include "server.h"

int main()
{
	evthread_use_windows_threads();
	//evthread_use_pthreads();
	event_enable_debug_mode();
	//替换记录日志的默认函数
	event_enable_debug_logging(EVENT_DBG_ALL);
	event_set_log_callback(log_callback);
	WinSockInit();
	//server();
	LibEvtServer libEvt;
	bool bRt = libEvt.init();
	if (bRt)
	{
		int nPort = 12345;
		libEvt.listen(&nPort);
	}
	while (1)
	{
		Sleep(1000);
	}
	return 0;
}
