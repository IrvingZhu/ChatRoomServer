#pragma once
#include <iostream>
#include "mysql.h"
// #pragma comment(lib, "libmysql.lib")
#pragma comment(a, "libmysql.a")

using namespace std;

int modifyPersonalInformation(const string &uid, const string &uname, const string &upassword)
{
	/*
	if true,return 1
	password not equal return -1
	uid unexist return -2
	connect error return 0
	*/
	MYSQL *con;
	//database configuartion
	char dbip[32] = "localhost";
	char dbuser[32] = "root";
	char dbpasswd[32] = "root";
	char dbname[32] = "chatroom";
	char tablename[32] = "people";
	char query[512] = "";

	int rt; //return value

	int count = 0;

	con = mysql_init((MYSQL *)0); //connect utility

	if (con != NULL && mysql_real_connect(con, dbip, dbuser, dbpasswd, dbname, 3306, NULL, 0))
	{
		if (!mysql_select_db(con, dbname))
		{
			cout << "********Select successfully the database!********\n"
				 << endl;
			con->reconnect = 1;
			mysql_query(con, "SET NAMES GBK"); // set the code
			// swprintf(wquery, L"update people set uid = '%s', uname = '%s', upassword = '%s'", uid.c_str(), uname.c_str(), upassword.c_str());
			string s_query("update people set uid = '");
			string s1("', uname = '");
			string s2("', upassword = '");
			string s3("'");
			s_query = s_query + uid + s1 + uname + s2 + upassword + s3;
			strcpy(query, s_query.c_str());			
			// auto query = convertToNarrowChars(wquery);
			rt = mysql_real_query(con, query, strlen(query));
			
			if (rt)
			{ // error
				cout << "ERROR making query: " << mysql_error(con) << " !!!" << endl;
				return -2;
			}
			else
			{ // success
				cout << "successfully update" << endl;
				return 1;
			}
		}
		else
		{
			cout << "**********choose the database fault*************" << endl;
			return 0;
		}
	}
	else
	{
		cout << "unable to connect the database,check your configuration!" << endl;
		return 0;
	}
}
