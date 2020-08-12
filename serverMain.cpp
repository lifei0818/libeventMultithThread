#include "common/function.h"
#include "server.h"

int main()
{
#ifdef WIN32
	evthread_use_windows_threads();
	WinSockInit();
#else
	evthread_use_pthreads();
#endif // WIN32

	event_enable_debug_mode();
	//替换记录日志的默认函数
	event_enable_debug_logging(EVENT_DBG_ALL);
	event_set_log_callback(log_callback);
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
        BaseLib::OS::sleep(SEC_TIME(1));
	}
	return 0;
}
