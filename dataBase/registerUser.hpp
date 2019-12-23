#pragma once
#include <iostream>
// #include "../utility/convert/convertToNarrowChars.hpp"
#include "mysql.h"
// #pragma comment(lib, "libmysql.lib")
#pragma comment(a, "libmysql.a")

using namespace std;

int registerUser(const string &uid, const string &uname, const string &upassword1)
{
	/*
	if true,return 1
	password not equal return -1
	uid exist return -2
	connect error return 0
	*/
	MYSQL *con;
	//database configuartion
	char dbip[32] = "localhost";
	char dbuser[32] = "root";
	char dbpasswd[32] = "root";
	char dbname[32] = "chatroom";
	char tablename[32] = "people";
	char query[256] = "";

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
			// swprintf(wquery, L"insert into people values('%s','%s','%s')", uid.c_str(), uname.c_str(), upassword1.c_str());
			string s_query("insert into people values('");
			string symbol_1("','");
			string symbol_2("')");
            s_query = s_query + uid + symbol_1 + uname + symbol_1 + upassword1 + symbol_2;
            strcpy(query, s_query.c_str());
			// auto query = convertToNarrowChars(wquery);
			cout << "the query content is: " << query << "\n" << endl;
			rt = mysql_real_query(con, query, strlen(query));
			
			if (rt)
			{ // error
				cout << "ERROR making query: " << mysql_error(con) << " !!!" << endl;
				return -2;
			}
			else
			{ //success
				cout << "successfully insert" << endl;
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