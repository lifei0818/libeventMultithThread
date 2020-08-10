#pragma once

#include<stdio.h>  
#include<string.h>  
 
#include<event.h>
#include<event2/util.h>
#include<event2/buffer.h>
#include <event2/thread.h>
#include<event2/listener.h> 
#include<event2/bufferevent.h>
bool WinSockInit();
void log_callback(int severity, const char *msg);