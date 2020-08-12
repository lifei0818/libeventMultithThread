#pragma once
#include "baselib.h"
#include "function.h"

#define PAGSIZE 1024
#define PAGHEAD string("TCPHEAD")
#define PAGTAIL string("TCPTAIL")
#define SEGCHAR string("x")

#define FILE_DESCRIBE "describe"
#define FILE_PATH  "filepath"
#define FILE_CONTENT "filecontent"

enum TCPSOURCETYPE { TCPSOURCE_SERVER, TCPSOURCE_CLIENT, TCPSOURCE_OTHER, TCPSOURCE_UNDEFINE };
enum TCPPACKAGETYPE { TCPPACKAGE_MSG, TCPPACKAGE_FILE };

struct PagTCP {
	std::string label;      // 消息或者文件类型的标签，一般为4个字符的描述
	TCPSOURCETYPE src;      // 消息来源：0:控制器服务端 1:示教器客户端 2:机器人库客户端 3:其他客户端设备，例如视觉
	TCPPACKAGETYPE type;    // 消息类型：0:指令 1:文件
	int num;                // 消息序号，消息包较大需要拆包时使用
	int allnum;             // 消息总数，消息包较大需要拆包时使用
	std::string content;	// 消息内容
};