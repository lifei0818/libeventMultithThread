#include "function.h"
#include "baselib.h"
#include <sstream>
#ifdef WIN32
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
#endif // WIN32

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

using namespace std;
int string_to_int(const std::string &p_content) {
	int m_res;
	if (p_content == "TRUE" || p_content == "ON") {
		m_res = 1;
	}
	else if (p_content == "FALSE" || p_content == "OFF") {
		m_res = 0;
	}
	else {
		stringstream ss;
		ss << p_content;
		ss >> m_res;
	}
	return m_res;
}
