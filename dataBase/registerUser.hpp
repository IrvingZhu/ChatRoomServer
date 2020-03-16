#pragma once
#include <iostream>
#include "mysql.h"
#include <mutex>
#include "./dataBaseMutex.hpp"

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
	int rt; //return value

	int count = 0;

	DBmtx.lock();
	con = mysql_init((MYSQL *)0); //connect utility

	if (con != NULL && mysql_real_connect(con, dbip, dbuser, dbpasswd, dbname, 3306, NULL, 0))
	{
		if (!mysql_select_db(con, dbname))
		{
			Log("********Select successfully the database!********\n", false);
			con->reconnect = 1;
			mysql_query(con, "SET NAMES GBK"); // set the code

			string s_query("insert into people values('");
			string symbol_1("','");
			string symbol_2("')");
            s_query = s_query + uid + symbol_1 + uname + symbol_1 + upassword1 + symbol_2;
            strcpy(query, s_query.c_str());

			Log("the query content is: " + s_query + "\n", false);
			rt = mysql_real_query(con, query, strlen(query));
			
			if (rt)
			{ // error
				std::string sql_er(mysql_error(con));
				Log("ERROR making query: " + sql_er + " !!!", false);
				DBmtx.unlock();
				clearDBqrConf();
				return -2;
			}
			else
			{ //success
				Log("successfully insert", false);
				DBmtx.unlock();
				clearDBqrConf();
				return 1;
			}
		}
		else
		{
			Log("**********choose the database fault*************", false);
			DBmtx.unlock();
			clearDBqrConf();
			return 0;
		}
	}
	else
	{
		Log("unable to connect the database,check your configuration!", false);
		DBmtx.unlock();
		clearDBqrConf();
		return 0;
	}
}