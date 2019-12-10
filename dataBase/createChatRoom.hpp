#pragma once
#include <iostream>
#include "../utility/convert/convertToNarrowChars.hpp"
#include "mysql.h"
// #pragma comment(lib, "libmysql.lib")
#pragma comment(a, "libmysql.a")

using namespace std;

<<<<<<< HEAD
int createChatRoom(const wstring &ChatID, const wstring &ChatName)
{
	/*
=======
int createChatRoom(wstring ChatID,wstring ChatName){
    /*
>>>>>>> CRS_Project_Mod
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

<<<<<<< HEAD
	if (con != NULL && mysql_real_connect(con, dbip, dbuser, dbpasswd, dbname, 3306, NULL, 0))
=======
    if (con != NULL && mysql_real_connect(con, dbip, dbuser, dbpasswd, dbname, 3306, NULL, 0))
>>>>>>> CRS_Project_Mod
	{ // connect
		if (!mysql_select_db(con, dbname))
		{
			cout << "********Select successfully the database!********\n"
				 << endl;
			con->reconnect = 1;
			mysql_query(con, "SET NAMES GBK"); // set code format
<<<<<<< HEAD
			swprintf(wquery, L"insert into chatroomset values('%s','%s')", ChatID.c_str(), ChatName.c_str());
=======
			swprintf(wquery, L"insert into chatroomset values('%s','%s')", ChatID.c_str(),ChatName.c_str());
>>>>>>> CRS_Project_Mod
			char *query;
			convertToNarrowChars(wquery, query);
			rt = mysql_real_query(con, query, strlen(query)); // qurey result
			if (rt)
			{
				cout << "ERROR making query: " << mysql_error(con) << " !!!" << endl;
				return 0;
			}
			else
<<<<<<< HEAD
			{ // success
				cout << "Success " << query << endl;
				return 1;
=======
			{   // success
				cout << "Success " << query << endl;
                return 1;
>>>>>>> CRS_Project_Mod
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