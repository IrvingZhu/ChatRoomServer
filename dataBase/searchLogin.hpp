#pragma once
#include <iostream>
#include "../utility/convert/convertToNarrowChars.hpp"
#include "mysql.h"
// #pragma comment(lib, "libmysql.lib")
#pragma comment(a, "libmysql.a")

using namespace std;

int searchLogin(const wstring &uname, const wstring &upassword)
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
	char tablename[32] = "people";
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
			wstring s_query(L"select upassword from people where UNAME = '");
			wstring symbol(L"'\n");
			s_query = s_query + uname + symbol;
			wcscpy(wquery, s_query.c_str());
			auto query = convertToNarrowChars(wquery);
			rt = mysql_real_query(con, query, strlen(query)); //qurey result
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
				res = new MYSQL_RES;
				res = mysql_store_result(con); // result
				row = mysql_fetch_row(res);

				if (!row)
					return 0;

				auto srcomp = convertToNarrowChars((wchar_t *)upassword.c_str());
				if (!strcmp(row[0], srcomp))
				{
					mysql_free_result(res);
					delete[] srcomp;
					cout << "This query's result is true" << endl;
					return 1; // successful
				}
				else
				{
					mysql_free_result(res);
					delete[] srcomp;
					cout << "This query's result is false" << endl;
					return 0;
				}
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