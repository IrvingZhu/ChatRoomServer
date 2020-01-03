#pragma once

#include <iostream>
#include <vector>
#include "mysql.h"
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

	con = mysql_init((MYSQL *)0); //connect

	if (con != NULL && mysql_real_connect(con, dbip, dbuser, dbpasswd, dbname, 3306, NULL, 0))
	{ // connect
		if (!mysql_select_db(con, dbname))
		{
			cout << "********Select successfully the database!********\n"
				 << endl;
			con->reconnect = 1;

			mysql_query(con, "SET NAMES GBK"); // set code format

			string s_query("select chatroomset.ChatName from chatroomset where ChatRID in (select peo_chat_r.ChatRID from peo_chat_r where peo_chat_r.uid = '");
			string symbol("')\n");
			s_query = s_query + info + symbol;
			cout << "The select sentences is: " << s_query << endl;
			strcpy(query, s_query.c_str());

			// auto query = convertToNarrowChars(wquery);
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

				while (row != NULL)
				{
					result.push_back(row[0]);
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