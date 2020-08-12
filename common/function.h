#pragma once

#include<stdio.h>  
#include<string.h>  
#include<string>
#include<event.h>
#include<event2/util.h>
#include<event2/buffer.h>
#include <event2/thread.h>
#include<event2/listener.h> 
#include<event2/bufferevent.h>
#ifdef WIN32
bool WinSockInit();
#endif // WIN32

void log_callback(int severity, const char *msg);
int string_to_int(const std::string &p_content);