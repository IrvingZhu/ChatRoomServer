#pragma once
#include <iostream>
#include "../utility/convert/convertToNarrowChars.hpp"
#include "mysql.h"
// #pragma comment(lib, "libmysql.lib")
#pragma comment(a, "libmysql.a")

using namespace std;

int createChatRoom(const wstring &ChatID, const wstring &ChatName)
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
	wchar_t wquery[256] = L"";

	int rt; //return value

	int count = 0;

	con = mysql_init((MYSQL *)0); //connect

	if (con != NULL && mysql_real_connect(con, dbip, dbuser, dbpasswd, dbname, 3306, NULL, 0))
	{ // connect
		if (!mysql_select_db(con, dbname))
		{
			cout << "********Select successfully the database!********\n"
				 << endl;
			con->reconnect = 1;
			mysql_query(con, "SET NAMES GBK"); // set code format
			// swprintf(wquery, L"insert into chatroomset values('%s','%s')", ChatID.c_str(), ChatName.c_str());
			wstring s_query(L"insert into chatroomset values('");
			wstring symbol_1(L"','");
			wstring symbol_2(L"')");
			s_query = s_query + ChatID + symbol_1 + ChatName + symbol_2;
			wcscpy(wquery, s_query.c_str());
			char *query = nullptr;
			convertToNarrowChars(wquery, query);
			rt = mysql_real_query(con, query, strlen(query)); // qurey result
			if (rt)
			{
				cout << "ERROR making query: " << mysql_error(con) << " !!!" << endl;
				delete[] query;
				return 0;
			}
			else
			{ // success
				cout << "Success " << query << endl;
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