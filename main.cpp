#include <stdio.h>
#include "baselib.h"
#include "DataHelper.h"
#include "StudentManager.h"
#define add_device ""
int main()
{
	testTask test;
	test.Open(2);
	int i = 10;
	while (i-->0)
	{
		cout << "here is main()\n" ;
		Sleep(1000);
	}
	return 0;
}
int testStudentDatahelper()
{
	string sqlstr( "select ipaddr from devices where ipaddr = '192.168.2.1'");
	/*DataHelper dataHelper;
	if (0 == dataHelper.Connect())
	{
		string strRt;
		if (0==dataHelper.Display(sqlstr, strRt))
		{

		}
	}*/
	BaseLib::TSingleton<DataHelper>::Instance()->Connect();
	StudentManager man;
	ostringstream oStr;
	ostringstream oStr1;
	string strsql;
	Json::StreamWriterBuilder builder;
	builder.settings_["indentation"] = "";
	std::unique_ptr<Json::StreamWriter> writeInfo(builder.newStreamWriter());
	Json::Value root, student,classJs,upfile,downfile;
	
	root["ipaddr"] = "192.168.2.2";
	root["deviceNumber"] = "T120_2";
	root["type"] = 0;
	writeInfo->write(root, &oStr);
	man.Device(oStr.str());
	man.Device(oStr.str(),2);
	cout << oStr.str() << endl;

	CCodeConver ch;
	string strName = "小王";
	int nLen = strName.length();
	LPWSTR wstr = ch.AnsiToUTF8(strName.c_str(), &nLen); 

	student["name"] = strName;
	student["className"] = "xxx_1";
	student["deviceNumber"] = "T120_1";
	student["deviceIP"] = "192.168.2.1";
	writeInfo->write(student, &oStr1);
	man.Student(oStr1.str());
	strsql = oStr1.str();
	man.Student(strsql,2);
	cout << strsql << endl;
	return 0;
	classJs["name"] = "xxx_1";
	oStr1.str("");
	writeInfo->write(classJs, &oStr1);
	man.Class(oStr1.str(),1);
	
	upfile["fileName"] = "filename1";
	upfile["studentName"] = "zhangsan";
	upfile["deviceNumber"] = "deviceNumber";
	upfile["className"] = "className";
	upfile["course"] = "course";
	
	upfile["type"] = 0;
	upfile["path"] = "c:\\\\test.program";
	oStr.str("");
	writeInfo->write(upfile, &oStr);
	man.UpFile(oStr.str());
	strsql = "{}";
	man.UpFile(strsql,2);
	cout <<"upfile:" <<strsql <<endl;


	downfile["fileName"] = "filename1";
	downfile["access"] = "zhangsan;class1";
	downfile["type"] = 0;
	downfile["path"] = "c:\\\\test1.program";
	downfile["course"] = "course";
	oStr.str("");
	writeInfo->write(downfile, &oStr);
	man.DownFile(oStr.str());
	man.DownFile(oStr.str(), 2);
	cout << oStr.str() << endl;

	string strOut;
	man.GetFiles("class2", "zhang", strOut);
	cout << "I can down load files:" << endl
		<< strOut << endl;
	return 0;
}

extern "C" {

VOID WINAPI SetCallBack()
{
}

BOOL
WINAPI
QueryFromUpper(DWORD dwCmdId, LPCSTR lpKey, OUT LPCSTR *lpResult)
{
	int nRt = -1;
	switch (dwCmdId)
	{
	//连接数据库
	case MYSQL_CONNECT:
	{
		nRt = BaseLib::TSingleton<DataHelper>::Instance()->Connect();
		break;
	}
	//无返回语句
	case MYSQL_EXXECUTE: {
		if (!lpKey)
		{
			return FALSE;
		}
		string strsql(lpKey);
		nRt = BaseLib::TSingleton<DataHelper>::Instance()->excuteSql(strsql);
		break;
	}
	//查询语句
	case MYSQL_GETDATA: {
		if (!lpKey)
		{
			return FALSE;
		}
		string strsql(lpKey);
		string strRt;
		nRt = BaseLib::TSingleton<DataHelper>::Instance()->Display(strsql, strRt);
		if (0 == nRt)
		{

			LPSTR lpChar = new CHAR[strRt.length() + 1];
			if (lpChar)
			{
				strcpy(lpChar, strRt.c_str());
				*lpResult = lpChar;
				return TRUE;
			}
		}
		break;
	}
	default:
		break;
	}
	if (nRt == 0)
		return TRUE;
	return FALSE;
}

BOOL
WINAPI
LpFree(IN LPCSTR& lpResult)
{
	if (lpResult)
	{
		delete[] lpResult;
		lpResult = NULL;
	}

	return TRUE;
}

int mainTest()
{
	string sqlstr;
	//建表
	sqlstr = "CREATE TABLE IF NOT EXISTS new_paper1(";
	sqlstr += "NewID int(11) NOT NULL AUTO_INCREMENT,";
	sqlstr += "NewCaption varchar(40) NOT NULL,";
	sqlstr += "NewContent text,";
	sqlstr += "NewTime datetime DEFAULT NULL,";
	sqlstr += "PRIMARY KEY(NewID)";
	sqlstr += ") ";

	DataHelper dataHelper;
	if (0==dataHelper.Connect())
	{
		dataHelper.excuteSql(sqlstr);

		for (int i = 0; i < 5; i++)
		{
			//插入数据
			sqlstr = "INSERT INTO test.new_paper1(NewID,NewCaption,NewContent,NewTime)";
			sqlstr += "VALUES(default,'jack','this a test','2017-01-11');";
			dataHelper.excuteSql(sqlstr);

		}
		//显示数据
		sqlstr = "SELECT NewID,NewCaption,NewContent,NewTime FROM test.new_paper1";
		string strRt;
		dataHelper.Display(sqlstr, strRt);
		cout << strRt << endl;

		//删除表
		sqlstr = "DROP TABLE test.new_paper1";
		dataHelper.excuteSql(sqlstr);
		
		dataHelper.close();
	}

	return 0;
}

} //extern "C"
