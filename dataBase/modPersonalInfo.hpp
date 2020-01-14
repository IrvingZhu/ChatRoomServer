#pragma once
#include <iostream>
#include "mysql.h"
// #pragma comment(lib, "libmysql.lib")
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

			// string s_query("update people set uid = '");
			// string s1("', uname = '");
			// string s2("', upassword = '");
			// string s3("'");
			// s_query = s_query + uid + s1 + uname + s2 + upassword + s3;
			// strcpy(query, s_query.c_str());

			string s1("' where uid = '");
			string s2("'");

			if (type == 0)
			{
				string s_query("update people set uname = '");

				s_query = s_query + info + s1 + uid + s2;
				cout << "the query sentences is : " << s_query << "\n";
				strcpy(query, s_query.c_str());
			}
			else if (type == 1)
			{
				string s_query("update people set upassword = '");

				s_query = s_query + info + s1 + uid + s2;
				cout << "the query sentences is : " << s_query << "\n";
				strcpy(query, s_query.c_str());
			}
			else
			{
				cout << "the type is input Error\n";
				return 0;
			}

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
