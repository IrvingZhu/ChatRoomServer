#pragma once
#include <iostream>
#include "../utility/convert/convertToNarrowChars.hpp"
#include "mysql.h"
// #pragma comment(lib, "libmysql.lib")
#pragma comment(a, "libmysql.a")

using namespace std;

int modifyPersonalInformation(const wstring &uid, const wstring &uname, const wstring &upassword)
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
	wchar_t wquery[256] = L"";

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
			wstring s_query(L"update people set uid = '");
			wstring s1(L"', uname = '");
			wstring s2(L"', upassword = '");
			wstring s3(L"'");
			s_query = s_query + uid + s1 + uname + s2 + upassword + s3;
			wcscpy(wquery, s_query.c_str());			
			auto query = convertToNarrowChars(wquery);
			rt = mysql_real_query(con, query, strlen(query));
			
			if (rt)
			{ // error
				cout << "ERROR making query: " << mysql_error(con) << " !!!" << endl;
				delete[] query;
				return -2;
			}
			else
			{ // success
				cout << "successfully update" << endl;
				delete[] query;
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
