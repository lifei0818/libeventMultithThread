#include "common/function.h"
#include "server.h"

int main()
{
	evthread_use_windows_threads();
	//evthread_use_pthreads();
	event_enable_debug_mode();
	//�滻��¼��־��Ĭ�Ϻ���
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
