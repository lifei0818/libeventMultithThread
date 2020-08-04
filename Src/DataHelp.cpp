#include "baselib.h"
#include "DataHelper.h"

DataHelper::DataHelper()
{
	//初始化
	if (0 == mysql_library_init(0, NULL, NULL)) {
		cout << "mysql_library_init() succeed" << endl;
		initMysql();
	}
	else {
		cout << "mysql_library_init() failed" << endl;
	}
}

DataHelper::~DataHelper()
{

	mysql_library_end();
}


int DataHelper::initMysql()
{
	if (NULL != mysql_init(&m_mydata)) {
		cout << "mysql_init(mydata) succeed" << endl;
	}
	else {
		cout << "mysql_init(mydata) failed" << endl;
		return -1;
	}
	//处理中文
	if (0 == mysql_options(&m_mydata, MYSQL_SET_CHARSET_NAME, "utf8mb4")) {
		cout << "mysql_options() succeed" << endl;
	}
	else {
		cout << "mysql_options() failed" << endl;
	}

}

int DataHelper::Connect()
{
	string strPath =  BaseLib::GetModuleFullPath(true);
	BaseLib::CIniFile iniFile((strPath + "\\config.ini").c_str());
	string strIp = iniFile.ReadString("Database", "ipaddr", "127.0.0.1");
	string strAccount = iniFile.ReadString("Database", "account", "root");
	string strPassword = iniFile.ReadString("Database", "password", "123123");
	string strName = iniFile.ReadString("Database", "name", "test");
	int port = iniFile.ReadInteger("Database", "port", 3306);

	//连接数据库
	if (NULL != mysql_real_connect(&m_mydata, strIp.c_str(), strAccount.c_str(), strPassword.c_str(), strName.c_str(), port, NULL, 0)) {
		cout << "mysql_real_connect() succeed" << endl;
	}
	else {
		
		cout << "mysql_real_connect() failed:"<< mysql_error(&m_mydata) << endl;

		mysql_close(&m_mydata);
		return -1;
	}
	return 0;
}


int DataHelper::excuteSql(std::string sqlstr)
{
	if (0 == mysql_query(&m_mydata, sqlstr.c_str())) {
		cout << "mysql_query() drop table succeed" << endl;
	}
	else {
		cout << "mysql_query() drop table failed" << mysql_error(&m_mydata) << endl;
		return -1;
	}

	return 0;
}


int DataHelper::Display(std::string sqlstr, string& strRt) {
	MYSQL_RES *result = NULL;
	stringstream sstream;
	if (0 == mysql_query(&m_mydata, sqlstr.c_str())) {
		cout << "mysql_query() select data succeed" << endl;
		result = mysql_store_result(&m_mydata);
		int rowcount = mysql_num_rows(result);
		cout << "row count:" << rowcount << endl;

		unsigned int fieldcount = mysql_num_fields(result);
		MYSQL_FIELD *field = NULL;
		for (unsigned int i = 0; i < fieldcount; i++) {
			field = mysql_fetch_field_direct(result, i);
			sstream << field->name << "\t\t";
		}
		sstream << endl;

		MYSQL_ROW row = NULL;
		row = mysql_fetch_row(result);
		while (NULL != row) {
			for (int i = 0; i < fieldcount; i++) {
				sstream << row[i] << "\t\t";
			}
			sstream << endl;
			row = mysql_fetch_row(result);
		}
		strRt = sstream.str();
	}
	else {
		cout << "mysql_query() select data failed" << mysql_error(&m_mydata) << endl;
		return -1;
	}

	mysql_free_result(result);
}

bool DataHelper::IsExistData(string strsql)
{
	MYSQL_RES *result = NULL;
	stringstream sstream;
	if (0 == mysql_query(&m_mydata, strsql.c_str())) {
		cout << "mysql_query() select data succeed" << endl;
		result = mysql_store_result(&m_mydata);
		int rowcount = mysql_num_rows(result);
		mysql_free_result(result);
		if (rowcount > 0)
		{
			return true;
		}
	}
	return false;
}

void DataHelper::close()
{
	mysql_close(&m_mydata);
}