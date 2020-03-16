#pragma once
#include <iostream>
#include "mysql.h"
#include <mutex>
#include "./dataBaseMutex.hpp"

#pragma comment(a, "libmysql.a")

using namespace std;

int searchLogin(const string &uname, const string &upassword)
{
	/*
	if true,return 1
	error return 0
	*/
	char tablename[7] = "people";

	int rt; //return value

	int count = 0;

	DBmtx.lock();
	con = mysql_init((MYSQL *)0); //connect

	if (con != NULL && mysql_real_connect(con, dbip, dbuser, dbpasswd, dbname, 3306, NULL, 0))
	{ // connect
		if (!mysql_select_db(con, dbname))
		{
			Log("********Select successfully the database!********\n", false);
			con->reconnect = 1;
			mysql_query(con, "SET NAMES GBK"); // set code format

			string s_query("select upassword from people where UNAME = '");
			string symbol("'\n");
			s_query = s_query + uname + symbol;
			strcpy(query, s_query.c_str());

			rt = mysql_real_query(con, query, strlen(query)); //qurey result
			if (rt)
			{
				std::string sql_er(mysql_error(con));
				Log("ERROR making query: " + sql_er + " !!!", false);
				DBmtx.unlock();
				clearDBqrConf();
				return 0;
			}
			else
			{ // success
				Log("Success " + s_query, false);
				res = new MYSQL_RES;
				res = mysql_store_result(con); // result
				row = mysql_fetch_row(res);

				if (!row){
					DBmtx.unlock();
					clearDBqrConf();
					return 0;
				}

				auto srcomp = upassword.c_str();
				if (!strcmp(row[0], srcomp))
				{
					mysql_free_result(res);
					Log("This query's result is true", false);
					DBmtx.unlock();
					clearDBqrConf();
					return 1; // successful
				}
				else
				{
					mysql_free_result(res);
					Log("This query's result is false", false);
					DBmtx.unlock();
					clearDBqrConf();
					return 0;
				}
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