#include "baselib.h"
#include "DataHelper.h"

DataHelper::DataHelper()
	:m_bConnect(FALSE)
{
	//初始化
	if (0 == mysql_library_init(0, NULL, NULL)) {
		DBGPRINT(DBG_ERROR,"mysql_library_init() succeed");
		initMysql();
	}
	else {
		DBGPRINT(DBG_ERROR, "mysql_library_init() failed");
	}
}

DataHelper::~DataHelper()
{

	mysql_library_end();
}


int DataHelper::initMysql()
{
	if (NULL != mysql_init(&m_mydata)) {
		DBGPRINT(DBG_ERROR,"mysql_init(mydata) succeed");
	}
	else {
		DBGPRINT(DBG_ERROR, "mysql_init(mydata) failed");
		return -1;
	}
	//处理中文
	if (0 == mysql_options(&m_mydata, MYSQL_SET_CHARSET_NAME, "utf8mb4")) {
		DBGPRINT(DBG_ERROR, "mysql_options() succeed");
	}
	else {
		DBGPRINT(DBG_ERROR, "mysql_options() failed");
	}
	return 0;
}

int DataHelper::Connect()
{
	if (IsConnect())
	{
		return 0;
	}
	string strPath =  BaseLib::GetModuleFullPath(true);
	BaseLib::CIniFile iniFile((strPath + "\\config.ini").c_str());
	string strIp = iniFile.ReadString("Database", "ipaddr", "127.0.0.1");
	string strAccount = iniFile.ReadString("Database", "account", "root");
	string strPassword = iniFile.ReadString("Database", "password", "123123");
	string strName = iniFile.ReadString("Database", "name", "test");
	int port = iniFile.ReadInteger("Database", "port", 3306);

	std::lock_guard<std::mutex> lock(m_data_mtx);
	//连接数据库
	if (NULL != mysql_real_connect(&m_mydata, strIp.c_str(), strAccount.c_str(), strPassword.c_str(), strName.c_str(), port, NULL, 0)) {
		DBGPRINT(DBG_ERROR, "mysql_real_connect() succeed");
	}
	else {
		DBGPRINT(DBG_ERROR, "mysql_real_connect() failed\n IP:"<< strIp<<" port:"<< port);
		
		mysql_close(&m_mydata);
		return -1;
	}
	m_bConnect = TRUE;
	return 0;
}


int DataHelper::excuteSql(std::string sqlstr)
{

	if (!IsConnect())
	{
		Connect();
	}

	std::lock_guard<std::mutex> lock(m_data_mtx);
	if (0 == mysql_query(&m_mydata, sqlstr.c_str())) {
		DBGPRINT(DBG_MYSQL_EXCUL, "mysql:"<<sqlstr);
	}
	else {
		DBGPRINT(DBG_ERROR, "mysql:" << sqlstr<< " err:" << mysql_error(&m_mydata));
		return -1;
	}

	return 0;
}


int DataHelper::Display(std::string sqlstr, string& strRt) {
	if (!IsConnect())
	{
		Connect();
	}
	MYSQL_RES *result = NULL;
	stringstream sstream;

	std::lock_guard<std::mutex> lock(m_data_mtx);
	if (0 == mysql_query(&m_mydata, sqlstr.c_str())) {
		result = mysql_store_result(&m_mydata);
		int rowcount = mysql_num_rows(result);

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
		DBGPRINT(DBG_MYSQL_EXCUL, "mysql:" << sqlstr<<"\n"<<strRt);
	}
	else {

		DBGPRINT(DBG_ERROR, "mysql:" << sqlstr << " err:" << mysql_error(&m_mydata));
		return -1;
	}

	mysql_free_result(result);
}

bool DataHelper::IsExistData(string strsql)
{
	MYSQL_RES *result = NULL;
	stringstream sstream;

	DBGPRINT(DBG_MYSQL_EXCUL, "mysql:" << strsql);
	std::lock_guard<std::mutex> lock(m_data_mtx);
	if (0 == mysql_query(&m_mydata, strsql.c_str())) {
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

	m_bConnect = FALSE;
	mysql_close(&m_mydata);
}


BOOL DataHelper::IsConnect()
{
	return m_bConnect;
}