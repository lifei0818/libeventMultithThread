#pragma once
#include "baselib.h"
#include "DataHelper.h"

class StudentManager
{
public:
	StudentManager();
	virtual ~StudentManager();

	//{
	//"ipaddr":"192.168.2.1",
	//"deviceNumber" : "T120_10",
	//"type" : 0	0:T120 ,1:18A
	//}
	//op:0:add 1:del 2.view(out strDevice)
	//return :Ê§°Ü<0
	int Device(string& strDevice, int op= 0);
	
	//{
	//"name":"ÕÅÈý"£¬
	//"className":"xxx_3",
	//"deviceNumber":"T120_1",
	//"deviceIP":"192.168.2.1"
	//}
	int Student(string& strStudent, int op = 0);

	//{"name":"xxx_1"}
	int Class(string& strClass, int op = 0);

	int UpFile(string& strClass, int op = 0);
	int DownFile(string& strClass, int op = 0);

	int GetFiles(const string& strClass, const string& strStudent, string& strOut);

	int excuteSql(string& strSql);

};

class testTask
	:public BaseLib::CTask
{
public:
	testTask() {}
	~testTask() {}

	INT32 Svc(void);
};

