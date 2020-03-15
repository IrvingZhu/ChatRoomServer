#pragma once
#include <iostream>
#include "mysql.h"
#include <mutex>
#include "./dataBaseMutex.hpp"

#pragma comment(a, "libmysql.a")

using namespace std;

int createChatRoom(const string &ChatID, const string &ChatName)
{
	/*
	if true,return 1
	error return 0
	*/
	MYSQL *con;
	MYSQL_RES *res;
	MYSQL_ROW row;
	//database configuartion
	char dbip[32] = "localhost";
	char dbuser[32] = "root";
	char dbpasswd[32] = "root";
	char dbname[32] = "chatroom";
	char query[512] = "";

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

			string s_query("insert into chatroomset values('");
			string symbol_1("','");
			string symbol_2("')");
			s_query = s_query + ChatID + symbol_1 + ChatName + symbol_2;
			strcpy(query, s_query.c_str());

			rt = mysql_real_query(con, query, strlen(query)); // qurey result

			if (rt)
			{
				std::string sql_er(mysql_error(con));
				Log("ERROR making query: " + sql_er + " !!!", false);
				DBmtx.unlock();
				return 0;
			}
			else
			{ // success
				std::string sql_qr(query);
				Log("Success " + sql_qr, false);
				DBmtx.unlock();
				return 1;
			}
		}
		else
		{
			Log("**********choose the database fault*************", false);
			DBmtx.unlock();
			return 0;
		}
	}
	else
	{
		Log("unable to connect the database,check your configuration!", false);
		DBmtx.unlock();
		return 0;
	}
}