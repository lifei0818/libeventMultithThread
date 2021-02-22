#ifndef SRC_DATAHELPER_H
#define SRC_DATAHELPER_H
#include <mysql.h>
#include <iostream>
#include <sstream>
#include <mutex>
#include "dataStructure.h"

using namespace std;

#define MYSQL_CONNECT 		0
#define MYSQL_EXXECUTE 		1
#define MYSQL_GETDATA		2

class EXPORTDLL DataHelper
{
public:
	DataHelper();
	~DataHelper();
	
	int Connect();
	int excuteSql(std::string sqlstr);
	int Display(std::string sqlstr, string& strRt);
	bool IsExistData(string strsql);
	void close();
	BOOL IsConnect();
private:
	int initMysql();
	MYSQL m_mydata;

	BOOL m_bConnect;
	std::mutex m_data_mtx;
};

#endif
