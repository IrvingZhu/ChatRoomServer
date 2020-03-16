#pragma once
#include <iostream>
#include "mysql.h"
#include <mutex>
#include "./dataBaseMutex.hpp"

#pragma comment(a, "libmysql.a")

using namespace std;

int modifyPersonalInformation(const string &uid, const string &info, int type)
{
	/*
	if true,return 1
	password not equal return -1
	uid unexist return -2
	connect error return 0
	for type,
	if 0,represent uname
	if 1,represent upsw
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

			string s1("' where uid = '");
			string s2("'");

			if (type == 0)
			{
				string s_query("update people set uname = '");

				s_query = s_query + info + s1 + uid + s2;
				Log("the query sentences is : " + s_query + "\n", false);
				strcpy(query, s_query.c_str());
			}
			else if (type == 1)
			{
				string s_query("update people set upassword = '");

				s_query = s_query + info + s1 + uid + s2;
				Log("the query sentences is : " + s_query + "\n", false);
				strcpy(query, s_query.c_str());
			}
			else
			{
				Log("the type is input Error\n", false);
				DBmtx.unlock();
				clearDBqrConf();
				return 0;
			}

			rt = mysql_real_query(con, query, strlen(query));

			if (rt)
			{   // error
				std::string sql_er(mysql_error(con));
				Log("ERROR making query: " + sql_er + " !!!", false);
				DBmtx.unlock();
				clearDBqrConf();
				return -2;
			}
			else
			{ // success
				Log("successfully update", false);
				DBmtx.unlock();
				clearDBqrConf();
				return 1;
			}
		}
		else
		{
			Log("*************choose the database fault*************", false);
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
