#pragma once

#include <iostream>
#include <vector>
#include "mysql.h"
#include <mutex>
#include "./dataBaseMutex.hpp"

#pragma comment(a, "libmysql.a")

using namespace std;

vector<string> searchUserJoinedRoom(const string &info){
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

	DBmtx.lock();
	con = mysql_init((MYSQL *)0); //connect

	if (con != NULL && mysql_real_connect(con, dbip, dbuser, dbpasswd, dbname, 3306, NULL, 0))
	{ // connect
		if (!mysql_select_db(con, dbname))
		{
			Log("********Select successfully the database!********\n", false);
			con->reconnect = 1;

			mysql_query(con, "SET NAMES GBK"); // set code format

			string s_query("select chatroomset.ChatName from chatroomset where ChatRID in (select peo_chat_r.ChatRID from peo_chat_r where peo_chat_r.uid = '");
			string symbol("')\n");
			s_query = s_query + info + symbol;
			Log("The select sentences is: " + s_query, false);
			strcpy(query, s_query.c_str());

			// auto query = convertToNarrowChars(wquery);
			rt = mysql_real_query(con, query, strlen(query)); // qurey result
			if (rt)
			{
				std::string sql_er(mysql_error(con));
				Log("ERROR making query: " + sql_er + " !!!", false);
				DBmtx.unlock();
				return result;
			}
			else
			{
				Log("Success " + s_query, false);
				res = new MYSQL_RES;
				res = mysql_store_result(con); // result
				row = mysql_fetch_row(res);	// row is two dimension array.

				while (row != NULL)
				{
					result.push_back(row[0]);
                    row = mysql_fetch_row(res);
				}
				mysql_free_result(res);
			}
			DBmtx.unlock();
			return result;
		}
		else
		{
			Log("*************choose the database fault*************", false);
			DBmtx.unlock();
			return result; // return null
		}
	}
	else
	{
		Log("unable to connect the database,check your configuration!", false);
		DBmtx.unlock();
		return result; // return null
	}
}