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
	std::string label;      // ��Ϣ�����ļ����͵ı�ǩ��һ��Ϊ4���ַ�������
	TCPSOURCETYPE src;      // ��Ϣ��Դ��0:����������� 1:ʾ�����ͻ��� 2:�����˿�ͻ��� 3:�����ͻ����豸�������Ӿ�
	TCPPACKAGETYPE type;    // ��Ϣ���ͣ�0:ָ�� 1:�ļ�
	int num;                // ��Ϣ��ţ���Ϣ���ϴ���Ҫ���ʱʹ��
	int allnum;             // ��Ϣ��������Ϣ���ϴ���Ҫ���ʱʹ��
	std::string content;	// ��Ϣ����
};