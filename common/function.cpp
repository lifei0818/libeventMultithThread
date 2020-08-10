#include "function.h"
#include "baselib.h"
bool WinSockInit() {
	WSADATA data = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &data))
		return false;
	if (LOBYTE(data.wVersion) != 2 || HIBYTE(data.wVersion) != 2) {
		WSACleanup();
		return false;
	}
	return true;
}
void log_callback(int severity, const char *msg)
{
	char szBuffer[512];

	FILE *pFd = fopen("./log.txt", "ab+");
	if (pFd == NULL)
		return;

	const char *severity_str;
	switch (severity)
	{
	case EVENT_LOG_DEBUG:
		severity_str = "debug";
		break;
	case EVENT_LOG_MSG:
		severity_str = "msg";
		break;
	case EVENT_LOG_WARN:
		severity_str = "warn";
		break;
	case EVENT_LOG_ERR:
		severity_str = "err";
		break;
	default:
		severity_str = "???";
		break;
	}

	snprintf(szBuffer, sizeof(szBuffer), "[%s]:%s\n", severity_str, msg);

	(void)fwrite(szBuffer, 1, strlen(szBuffer), pFd);

	fclose(pFd);
}