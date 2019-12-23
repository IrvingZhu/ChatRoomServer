#pragma once

#include <iostream>
#include <vector>
// #include "../utility/convert/convertToNarrowChars.hpp"
#include "mysql.h"
// #pragma comment(lib, "libmysql.lib")
#pragma comment(a, "libmysql.a")

using namespace std;

int peo_table_element_num = 3;
int Rela_num = 3;
int ChatRoom_num = 2;

// if query true,return a full vector,
// else return a null set of vector.

vector<string> searchAllOfChatRoom(const string &ChatID)
{
	vector<string> result;
	
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

	con = mysql_init((MYSQL *)0); //connect

	if (con != NULL && mysql_real_connect(con, dbip, dbuser, dbpasswd, dbname, 3306, NULL, 0))
	{ // connect
		if (!mysql_select_db(con, dbname))
		{
			cout << "********Select successfully the database!********\n"
				 << endl;
			con->reconnect = 1;
			mysql_query(con, "SET NAMES GBK"); // set code format
			sprintf(query, "select * from chatroomset where ChatID = '%s'", ChatID.c_str());
			// auto query = convertToNarrowChars(wquery);
			rt = mysql_real_query(con, query, strlen(query)); // qurey result
			if (rt)
			{
				cout << "ERROR making query: " << mysql_error(con) << " !!!" << endl;
				delete[] query;
				return result;
			}
			else
			{
				cout << "Success " << query << endl;
				delete[] query;
				res = new MYSQL_RES;
				res = mysql_store_result(con); // result
				row = mysql_fetch_row(res);	// row is two dimension array.
				int i = 0;
				while (i < ChatRoom_num)
				{
					result.push_back(row[i]);
					i++;
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

vector<string> searchAllOfRela(const string &PID, const string &CID)
{
	vector<string> result;

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

	con = mysql_init((MYSQL *)0); //connect

	if (con != NULL && mysql_real_connect(con, dbip, dbuser, dbpasswd, dbname, 3306, NULL, 0))
	{ // connect
		if (!mysql_select_db(con, dbname))
		{
			cout << "********Select successfully the database!********\n"
				 << endl;
			con->reconnect = 1;
			mysql_query(con, "SET NAMES GBK"); // set code format
			// swprintf(wquery, L"select * from peo_chat_r where UID = '%s'", PID.c_str());
			string s_query("select * from peo_chat_r where UID = '");
			string symbol("'\n");
			s_query = s_query + PID + symbol;
			strcpy(query, s_query.c_str());
			// auto query = convertToNarrowChars(wquery);
			rt = mysql_real_query(con, query, strlen(query)); // qurey result
			if (rt)
			{
				cout << "ERROR making query: " << mysql_error(con) << " !!!" << endl;
				delete[] query;
				return result;
			}
			else
			{
				cout << "Success " << query << endl;
				delete[] query;
				res = new MYSQL_RES;
				res = mysql_store_result(con); // result
				row = mysql_fetch_row(res);	// row is two dimension array.
				int i = 0;
				while (i < Rela_num)
				{
					result.push_back(row[i]);
					i++;
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

vector<string> searchAllOfPeople(const string &Search_info, int type)
{
	// uid 0
	// uname 1
	vector<string> result;

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

	con = mysql_init((MYSQL *)0); //connect

	if (con != NULL && mysql_real_connect(con, dbip, dbuser, dbpasswd, dbname, 3306, NULL, 0))
	{ // connect
		if (!mysql_select_db(con, dbname))
		{
			cout << "********Select successfully the database!********\n"
				 << endl;
			con->reconnect = 1;
			mysql_query(con, "SET NAMES GBK"); // set code format
			if (type == 0)
			{
				// uid
				string s_query("select * from people where UID = '");
				string symbol("'\n");
				s_query = s_query + Search_info + symbol;
				strcpy(query, s_query.c_str());
				// swprintf(wquery, L"select * from people where UID = '%s'", Search_info.c_str());
			}
			else if (type == 1)
			{
				// uname
				string s_query("select * from people where UNAME = '");
				string symbol("'\n");
				s_query = s_query + Search_info + symbol;
				// swprintf(wquery, L"select * from people where UNAME = '%s'", Search_info.c_str());
				strcpy(query, s_query.c_str());
			}
			else{
				return result;
			}
			// auto query = convertToNarrowChars(wquery);
			rt = mysql_real_query(con, query, strlen(query)); // qurey result
			if (rt)
			{
				cout << "ERROR making query: " << mysql_error(con) << " !!!" << endl;
				delete[] query;
				return result;
			}
			else
			{
				cout << "Success " << query << endl;
				delete[] query;
				res = new MYSQL_RES;
				res = mysql_store_result(con); // result
				row = mysql_fetch_row(res);	// row is two dimension array.
				int i = 0;
				while (i < peo_table_element_num)
				{
					result.push_back(row[i]);
					i++;
					// row = mysql_fetch_row(res);
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