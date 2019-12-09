#pragma once

#include <iostream>
#include <vector>
#include "../utility/convert/convertToNarrowChars.hpp"
#include "mysql.h"
// #pragma comment(lib, "libmysql.lib")
#pragma comment(a, "libmysql.a")

using namespace std;

int peo_table_element_num = 3;
int Rela_num = 3;
int ChatRoom_num = 2;

// if query true,return a full vector,
// else return a null set of vector.

vector<char *> searchAllOfChatRoom(const wstring &ChatID)
{
	vector<char *> result;
	result.resize(ChatRoom_num);
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
			swprintf(wquery, L"select * from chatroomset where ChatID = '%s'", ChatID.c_str());
			char *query;
			convertToNarrowChars(wquery, query);
			rt = mysql_real_query(con, query, strlen(query)); // qurey result
			if (rt)
			{
				cout << "ERROR making query: " << mysql_error(con) << " !!!" << endl;
				return result;
			}
			else
			{
				cout << "Success " << query << endl;
				res = new MYSQL_RES;
				res = mysql_store_result(con); // result
				row = mysql_fetch_row(res);	// row is two dimension array.
				int i = 0;
				while (i < ChatRoom_num)
				{
					result.push_back(row[i]);
					i++;
					row = mysql_fetch_row(res);
				}
				mysql_free_result(res);
			}
			return result;
		}
		else
		{
			cout << "**********choose the database fault*************" << endl;
			return result; // return null
		}
	}
	else
	{
		cout << "unable to connect the database,check your configuration!" << endl;
		return result; // return null
	}
}

vector<char *> searchAllOfRela(wstring PID, wstring CID)
{
	vector<char *> result;
	result.resize(Rela_num);

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
			swprintf(wquery, L"select * from peo_chat_r where UID = '%s'", PID.c_str());
			char *query;
			convertToNarrowChars(wquery, query);
			rt = mysql_real_query(con, query, strlen(query)); // qurey result
			if (rt)
			{
				cout << "ERROR making query: " << mysql_error(con) << " !!!" << endl;
				return result;
			}
			else
			{
				cout << "Success " << query << endl;
				res = new MYSQL_RES;
				res = mysql_store_result(con); // result
				row = mysql_fetch_row(res);	// row is two dimension array.
				int i = 0;
				while (i < Rela_num)
				{
					result.push_back(row[i]);
					i++;
					row = mysql_fetch_row(res);
				}
				mysql_free_result(res);
			}
			return result;
		}
		else
		{
			cout << "**********choose the database fault*************" << endl;
			return result; // return null
		}
	}
	else
	{
		cout << "unable to connect the database,check your configuration!" << endl;
		return result; // return null
	}
}

vector<char *> searchAllOfPeople(wstring Search_info, int type)
{
	// uid 0
	// uname 1
	vector<char *> result;
	result.resize(peo_table_element_num);

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
			switch (type)
			{
			case 0: // uid
				swprintf(wquery, L"select * from people where UID = '%s'", Search_info.c_str());
				break;
			case 1: // uname
				swprintf(wquery, L"select * from people where UNAME = '%s'", Search_info.c_str());
				break;
			default:
				break;
			}
			char *query;
			convertToNarrowChars(wquery, query);
			rt = mysql_real_query(con, query, strlen(query)); // qurey result
			if (rt)
			{
				cout << "ERROR making query: " << mysql_error(con) << " !!!" << endl;
				return result;
			}
			else
			{
				cout << "Success " << query << endl;
				res = new MYSQL_RES;
				res = mysql_store_result(con); // result
				row = mysql_fetch_row(res);	// row is two dimension array.
				int i = 0;
				while (i < peo_table_element_num)
				{
					result.push_back(row[i]);
					i++;
					row = mysql_fetch_row(res);
				}
				mysql_free_result(res);
			}
			return result;
		}
		else
		{
			cout << "**********choose the database fault*************" << endl;
			return result; // return null
		}
	}
	else
	{
		cout << "unable to connect the database,check your configuration!" << endl;
		return result; // return null
	}
}