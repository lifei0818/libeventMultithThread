#include "baselib.h"
#include "StudentManager.h"

#include <sstream>
#include <string.h>

StudentManager::StudentManager()
{
}

StudentManager::~StudentManager()
{
}

int StudentManager::Device(string& strDevice, int op )
{
	int nRt = -1;
	Json::CharReaderBuilder jsReader;
	std::unique_ptr<Json::CharReader> const readInfo(jsReader.newCharReader());

	Json::Value device;
	string err;
	int nLen = strDevice.length();
	if (readInfo->parse(strDevice.c_str(), strDevice.c_str() + nLen, &device, &err)) {
		string strIP,strNumber;
		strIP = device["ipaddr"].asString();
		strNumber = device["deviceNumber"].asString();
		int type = device["type"].asInt();
		//device[]

		string strsql = "select deviceNumber,ipaddr from devices where ipaddr='";
		strsql += strIP;
		strsql += "' or deviceNumber='";
		strsql += strNumber + "';";

		bool bRt = BaseLib::TSingleton<DataHelper>::Instance()->IsExistData(strsql);
		if ((op==0)&&!bRt)
		{
			strsql = "insert into devices(ipaddr,deviceNumber,type) values()";
			char buff[128];
			sprintf(buff, "insert into devices(ipaddr,deviceNumber,type) values('%s','%s',%d)",strIP.c_str(),strNumber.c_str(),type );
			nRt = BaseLib::TSingleton<DataHelper>::Instance()->excuteSql(buff);
		}
		else if ((op == 1) &&bRt ) {
			strsql = "delete from devices where ipaddr='" + strIP + "';";
			nRt = BaseLib::TSingleton<DataHelper>::Instance()->excuteSql(strsql);
			strsql = "delete from devices where ipaddr='" + strNumber + "';";
			nRt = BaseLib::TSingleton<DataHelper>::Instance()->excuteSql(strsql);
		}
		else if (2 == op) {
			if (!device["ipaddr"].isNull()&&!strIP.empty())
			{
				strsql = "select * from devices where ipaddr='" + strIP + "';";
			}
			else if (!device["deviceNumber"].isNull()&&!strNumber.empty())
			{
				strsql = "select * from devices where deviceNumber='" + strNumber + "';";
			}
			else if (!device["type"].isNull()&&type>=0)
			{
				char buff[128] = { 0 };
				sprintf(buff,"select * from devices where type='%d';",type);
				strsql = buff;
			}
			else {
				strsql = "select * from devices ;";
			}
			nRt = BaseLib::TSingleton<DataHelper>::Instance()->Display(strsql, strDevice);
		}
	}
	return nRt;
}

int StudentManager::Student(string& strStudent, int op)
{
	int nRt = -1;
	Json::CharReaderBuilder jsReader;
	std::unique_ptr<Json::CharReader> const readInfo(jsReader.newCharReader());

	Json::Value student;
	string err;
	int nLen = strStudent.length();
	if (readInfo->parse(strStudent.c_str(), strStudent.c_str() + nLen, &student, &err)) {
		string strName = student["name"].asString();
		string strClassName = student["className"].asString();
		string strDeviceIP = student["deviceIP"].asString();
		string strDeviceNumber = student["deviceNumber"].asString();
		if (strDeviceIP.empty() && strDeviceIP.empty())
		{
			nRt = -1;
		}
		else
		{
			string strsql("select className,name from studentinfo where class='");
			char buff[128] = {0};
			sprintf(buff,"select className, name from studentinfo where className = '%s' and name='%s';",strClassName.c_str(),strName.c_str());
			///该生是否存在
			int bRt = BaseLib::TSingleton<DataHelper>::Instance()->IsExistData(buff);
			if (!bRt&&op==0)
			{
				memset(buff,0,sizeof(buff));
				sprintf(buff, "insert into studentinfo (name,className,deviceNumber,deviceIP) \
					values('%s','%s','%s','%s');",strName.c_str(), strClassName.c_str(), strDeviceNumber.c_str(), strDeviceIP.c_str());
				string strsql = "select deviceNumber,ipaddr from devices where ipaddr='";
				strsql += strDeviceIP;
				strsql += "' or numble='";
				strsql += strDeviceNumber + "';";
				//设备是否存在
				bRt = BaseLib::TSingleton<DataHelper>::Instance()->IsExistData(strsql.c_str());
				if (bRt)
				{
					nRt = BaseLib::TSingleton<DataHelper>::Instance()->excuteSql(buff);
				}
				else
				{
					cout << strDeviceNumber << "/" << strDeviceIP << " that device is not exist!" << endl;
				}
			}
			else if(bRt &&(op==1))
			{
				strsql = "delete from studentinfo where name='";
				strsql += strName + "' and className='";
				strsql += strClassName + "';";
				nRt = BaseLib::TSingleton<DataHelper>::Instance()->excuteSql(strsql);
			}else if (2 == op) {
				if (!student["name"].isNull())
				{
					strsql = "select * from studentinfo where name='" + strName + "';";
				}
				else if (!student["className"].isNull())
				{
					strsql = "select * from studentinfo where className='" + strClassName + "';";
				}
				else if (!student["deviceIP"].isNull())
				{
					strsql = "select * from studentinfo where deviceIP='" + strDeviceIP + "';";
				}
				else if (!student["deviceNumber"].isNull())
				{
					strsql = "select * from studentinfo where deviceNumber='" + strDeviceNumber + "';";
				}
				else {
					strsql = "select * from studentinfo;";
				}
				nRt = BaseLib::TSingleton<DataHelper>::Instance()->Display(strsql, strStudent);
			}
		}
	}
	return nRt;
}

int StudentManager::Class(string& strClass, int op)
{
	int nRt = -1;
	Json::CharReaderBuilder jsReader;
	std::unique_ptr<Json::CharReader> const readInfo(jsReader.newCharReader());

	Json::Value classJs;
	string err;
	int nLen = strClass.length();
	if (readInfo->parse(strClass.c_str(), strClass.c_str() + nLen, &classJs, &err)) {
		string strName = classJs["name"].asString();
		string strsql("select * from class where name='");
		strsql += strName + "';";
		bool bRt = BaseLib::TSingleton<DataHelper>::Instance()->IsExistData(strsql);
		if (!bRt&&op==0)
		{
			strsql = "insert into class(name) values('" + strName + "');";
			nRt = BaseLib::TSingleton<DataHelper>::Instance()->excuteSql(strsql);
		}
		else if (bRt && op == 1) {
			strsql = "delete from class where name='";
			strsql += strName + "';";
			nRt = BaseLib::TSingleton<DataHelper>::Instance()->excuteSql(strsql);
		}
		else if (2 == op)
		{
			strsql = "select * from class;";
			nRt = BaseLib::TSingleton<DataHelper>::Instance()->Display(strsql, strClass);
		}
	}
	return nRt;
}

int StudentManager::UpFile(string& strClass, int op)
{
	int nRt = -1;
	Json::CharReaderBuilder jsReader;
	std::unique_ptr<Json::CharReader> const readInfo(jsReader.newCharReader());

	Json::Value upfile;
	string err;
	int nLen = strClass.length();
	if (readInfo->parse(strClass.c_str(), strClass.c_str() + nLen, &upfile, &err)) {
		string strFileName = upfile[FILE_NAME].asString();
		string strStudentName = upfile[FILE_STUDENT].asString();
		string strClassName = upfile[FILE_CLASS].asString();
		string strPath = upfile[FILE_PATH].asString();
		string strDeviceNumber = upfile[DEVICE_NUMBER].asString();
		string strCourse = upfile[FILE_COURES].asString();
		int nType = upfile[FILE_TYPE].asInt();
		ostringstream oStr;
		string strsql;
		if (op == 0)
		{
			if (!BaseLib::IsFileExist(strPath.c_str()))
			{
				return 0;
			}
			BaseLib::StringReplaceAll(strPath, "\\", "\\\\");
			strsql = "insert into upfile(fileName,studentName,className,path,deviceNumber,course,type,time) values(";
			strsql += "'" + strFileName +"',";
			strsql += "'" + strStudentName +"',";
			strsql += "'" + strClassName +"',";
			strsql += "'" + strPath +"',";
			strsql += "'" + strDeviceNumber +"',";
			strsql += "'" + strCourse + "',";
			oStr << strsql<<nType<<",now());";
			nRt = BaseLib::TSingleton<DataHelper>::Instance()->excuteSql(oStr.str());
		}
		else if(op == 1)
		{
			strsql = "delete from upfile where path='" + strPath + "';";
			nRt = BaseLib::TSingleton<DataHelper>::Instance()->excuteSql(strsql);
		}
		else if (op == 2)
		{
			if (!upfile["studentName"].isNull())
			{
				strsql = "select * from upfile where studentName='" + strStudentName + "';";
			}
			else if (!upfile["className"].isNull())
			{
				strsql = "select * from upfile where className='" + strClassName + "';";
			}
			else if (!upfile["course"].isNull())
			{
				strsql = "select * from upfile where course='" + strCourse + "';";
			}
			else if (!upfile["deviceNumber"].isNull())
			{
				strsql = "select * from upfile where deviceNumber='" + strDeviceNumber + "';";
			}
			else if (!upfile["type"].isNull())
			{
				char buff[128] = { 0 };
				sprintf(buff, "select * from upfile where type='%d';", nType);
				strsql = buff;
			}
			else {
				strsql = "select * from upfile ;";
			}
			nRt = BaseLib::TSingleton<DataHelper>::Instance()->Display(strsql, strClass);
		}
	}
	return nRt;
}

int StudentManager::DownFile(string& strClass, int op)
{
	int nRt = -1;
	Json::CharReaderBuilder jsReader;
	std::unique_ptr<Json::CharReader> const readInfo(jsReader.newCharReader());

	Json::Value downfile;
	string err;
	int nLen = strClass.length();
	if (readInfo->parse(strClass.c_str(), strClass.c_str() + nLen, &downfile, &err)) {
		string strFileName = downfile["fileName"].asString();
		string strPath = downfile["path"].asString();
		string strAccess = downfile["access"].asString();
		int nType = downfile["type"].asInt();
		string strCourse = downfile["course"].asString();
		ostringstream oStr;
		string strsql;
		if (op == 0)
		{
			strsql = "insert into downfile(fileName,path,access,course,type,time) values(";
			strsql += "'" + strFileName + "',";
			strsql += "'" + strPath + "',";
			strsql += "'" + strAccess + "',";
			strsql += "'" + strCourse + "',";
			oStr << strsql << nType << ",now());";
			nRt = BaseLib::TSingleton<DataHelper>::Instance()->excuteSql(oStr.str());
		}
		else if (op == 1)
		{
			strsql = "delete from downfile where path='" + strPath + "';";
			nRt = BaseLib::TSingleton<DataHelper>::Instance()->excuteSql(strsql);
		}
		else if (op == 2)
		{
			if (!downfile["course"].isNull())
			{
				strsql = "select * from downfile where course='" + strCourse + "';";
			}
			else if (!downfile["fileName"].isNull())
			{
				strsql = "select * from downfile where fileName='" + strFileName + "';";
			}
			else if (!downfile["type"].isNull())
			{
				char buff[128] = { 0 };
				sprintf(buff, "select * from downfile where type='%d';", nType);
				strsql = buff;
			}
			else {
				strsql = "select * from downfile ;";
			}
			nRt = BaseLib::TSingleton<DataHelper>::Instance()->Display(strsql, strClass);
		}
	}
	return nRt;
}

int StudentManager::GetFiles(const string & strClass, const string & strStudent, string & strOut)
{
	int nRt = -1;
	string strSql("select fileName, path, 1 from upfile where ");
	strSql += "className='" + strClass + "' and studentName='"+ strStudent + "'";;
	strSql += " union ";
	strSql += " select fileName, path, 2 from downfile where access REGEXP '"+strClass+"';";
	nRt = BaseLib::TSingleton<DataHelper>::Instance()->Display(strSql, strOut);
	return nRt;
}

int StudentManager::excuteSql(string & strSql)
{
	int nRt = -1;
	nRt = BaseLib::TSingleton<DataHelper>::Instance()->Display(strSql, strSql);
	return nRt;
}

INT32 testTask::Svc(void)
{
	int i = 10;
	while (i--) {
		Sleep(1000);
		cout << "here is test;Svc\t" <<PrintThreadId()<< endl;
	}
	Close();
	return TASK_BREAK;
}
