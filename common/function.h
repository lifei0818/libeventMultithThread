#pragma once

#include<event.h>
#include<event2/bufferevent.h>
#include<event2/buffer.h>
#include<event2/util.h>
#include <event2/thread.h>
bool WinSockInit();
void log_callback(int severity, const char *msg);