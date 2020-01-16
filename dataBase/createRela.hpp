#pragma once
#include <iostream>
// #include "../utility/convert/convertToNarrowChars.hpp"
#include "mysql.h"
// #pragma comment(lib, "libmysql.lib")
#pragma comment(a, "libmysql.a")

using namespace std;

int createRela(const string &ID, const string &UID, const string &ChatID)
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

    con = mysql_init((MYSQL *)0); //connect

    if (con != NULL && mysql_real_connect(con, dbip, dbuser, dbpasswd, dbname, 3306, NULL, 0))
    { // connect
        if (!mysql_select_db(con, dbname))
        {
            cout << "********Select successfully the database!********\n"
                 << endl;
            con->reconnect = 1;
            mysql_query(con, "SET NAMES GBK"); // set code format
            // swprintf(wquery, L"insert into peo_chat_r values('%s', '%s','%s')", ID.c_str(), UID.c_str(), ChatID.c_str());
			string s_query("insert into peo_chat_r values('");
			string symbol_1("','");
			string symbol_2("')");
            s_query = s_query + ID + symbol_1 + UID + symbol_1 + ChatID + symbol_2;
            cout << "the query sentences is: " << s_query << "\n";
            strcpy(query, s_query.c_str());              
            // auto query = convertToNarrowChars(wquery);
            rt = mysql_real_query(con, query, strlen(query)); // qurey result
            
            if (rt)
            {
                cout << "ERROR making query: " << mysql_error(con) << " !!!" << endl;
                return 0;
            }
            else
            { // success
                cout << "Success " << query << endl;
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