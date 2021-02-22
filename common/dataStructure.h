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
	std::string label;      // ��Ϣ�����ļ����͵ı�ǩ��һ��Ϊ4���ַ�������
	TCPSOURCETYPE src;      // ��Ϣ��Դ��0:����������� 1:ʾ�����ͻ��� 2:�����˿�ͻ��� 3:�����ͻ����豸�������Ӿ�
	TCPPACKAGETYPE type;    // ��Ϣ���ͣ�0:ָ�� 1:�ļ�
	int num;                // ��Ϣ��ţ���Ϣ���ϴ���Ҫ���ʱʹ��
	int allnum;             // ��Ϣ��������Ϣ���ϴ���Ҫ���ʱʹ��
	std::string content;	// ��Ϣ����
};