#pragma once
#include "baselib.h"

#define PAGSIZE 1024
#define PAGHEAD string("TCPHEAD")
#define PAGTAIL string("TCPTAIL")
#define SEGCHAR string("x")
#define EXPORTDLL __declspec(dllexport)

#define FILE_DESCRIBE	"describe"
#define FILE_NAME		"filename"
#define FILE_PATH		"filepath"
#define FILE_TYPE		"filetype"
#define FILE_CLASS		"className"
#define FILE_STUDENT	"studentName"
#define FILE_COURES		"coures"
#define FILE_CONTENT	"filecontent"
#define FILE_TEMPLATE	"fileistemplate"

#define DEVICE_NUMBER	"devicenumber"
#define DEVICE_IP		"deviceip"

#define CLASS_INFO		"classinfo"


#define FILE_LIST		"filelist"


//0:HR 1:ABB 2:FANUC
#define EXT_HR			".program"
#define EXT_ABB			".abb"
#define EXT_FANUC		".fanuc"

#define DECOLLATOR		";"

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