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
			// wchar_t *name = new wchar_t[64];
			// wcscpy(name, uname.c_str());
			swprintf(wquery, L"select upassword from people where UNAME = '%s'", uname.c_str());
			char *query = nullptr;
			convertToNarrowChars(wquery, query);
			rt = mysql_real_query(con, query, strlen(query)); //qurey result
			if (rt)
			{
				cout << "ERROR making query: " << mysql_error(con) << " !!!" << endl;
				return 0;
			}
			else
			{ // success
				cout << "Success " << query << endl;
				res = new MYSQL_RES;
				res = mysql_store_result(con); // result
				row = mysql_fetch_row(res);

				if (!row)
					return 0;

				string rcomp;
				rcomp = (char *)row[0];
				char *srcomp;
				convertToNarrowChars((wchar_t *)upassword.c_str(), srcomp);
				if (!strcmp(rcomp.c_str(), srcomp))
					return 1; // successful
				else
					return 0;
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