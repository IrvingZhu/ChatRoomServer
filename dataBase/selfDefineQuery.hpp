#pragma once

#include <iostream>
#include "mysql.h"
#include <vector>
#include <mutex>
#include "./dataBaseMutex.hpp"

#pragma comment(a, "libmysql.a")

std::vector<vector<std::string>> selfDefineQuery(char *wquery, int ret_record, int para)
{

    // ret_record is the number of the tuple of return res in the database query.
    // para is the number of the tuple's component in each row.

    std::vector<vector<std::string>> result;

    MYSQL *con;
    MYSQL_RES *res;
    MYSQL_ROW row;
    std::mutex mtx;
    //database configuartion
    char dbip[32] = "localhost";
    char dbuser[32] = "root";
    char dbpasswd[32] = "root";
    char dbname[32] = "chatroom";
    // char *mquery = new char[512];
    char mquery[512];
    memset(mquery, 0, strlen(mquery));
    strcpy(mquery, wquery);

    int rt; //return value

    int count = 0;

    mtx.lock();
	con = mysql_init((MYSQL *)0); //connect

    if (con != NULL && mysql_real_connect(con, dbip, dbuser, dbpasswd, dbname, 3306, NULL, 0))
    { // connect
        if (!mysql_select_db(con, dbname))
        {
            Log("********Select successfully the database!********\n", false);
            con->reconnect = 1;
            mysql_query(con, "SET NAMES GBK"); // set code format

            rt = mysql_real_query(con, mquery, strlen(mquery)); // qurey result
            if (rt)
            {
                std::string sql_er(mysql_error(con));
				Log("ERROR making query: " + sql_er + " !!!", false);
                DBmtx.unlock();
                return result;
            }
            else
            {
                std::cout << "Success " << mquery << std::endl;
                res = mysql_store_result(con); // result

                int ret_count = 0, para_count = 0;

                while (ret_count < ret_record)
                {
                    if (!(row = mysql_fetch_row(res)))
                        break;
                    ret_count++;

                    vector<std::string> each_per_res;
                    para_count = 0; // initial three variable
                    while (para_count < para)
                    {
                        std::string temp(row[para_count]);
                        each_per_res.push_back(temp);
                        para_count++;
                    }
                    result.push_back(each_per_res);
                }
                mysql_free_result(res);
            }
            DBmtx.unlock();
            return result; // return true
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