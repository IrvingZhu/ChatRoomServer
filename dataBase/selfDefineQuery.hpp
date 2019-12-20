#pragma once

#include <iostream>
#include "mysql.h"
#include <vector>
#include "../utility/convert/convertToNarrowChars.hpp"
// #pragma comment(lib, "libmysql.lib")
#pragma comment(a, "libmysql.a")

std::vector<std::string> selfDefineQuery(wchar_t *wquery, int ret_record, int para)
{
    std::vector<std::string> result;

    MYSQL *con;
    MYSQL_RES *res;
    MYSQL_ROW row;
    //database configuartion
    char dbip[32] = "localhost";
    char dbuser[32] = "root";
    char dbpasswd[32] = "root";
    char dbname[32] = "chatroom";
    wchar_t *mwquery = new wchar_t[256];
    memset(mwquery, 0, wcslen(mwquery));
    wcscpy(mwquery, wquery);
    delete[] mwquery;

    int rt; //return value

    int count = 0;

    con = mysql_init((MYSQL *)0); //connect

    if (con != NULL && mysql_real_connect(con, dbip, dbuser, dbpasswd, dbname, 3306, NULL, 0))
    { // connect
        if (!mysql_select_db(con, dbname))
        {
            std::cout << "********Select successfully the database!********\n"
                      << std::endl;
            con->reconnect = 1;
            mysql_query(con, "SET NAMES GBK"); // set code format
            auto query = convertToNarrowChars(wquery);
            rt = mysql_real_query(con, query, strlen(query)); // qurey result
            if (rt)
            {
                std::cout << "ERROR making query: " << mysql_error(con) << " !!!" << std::endl;
                delete[] query;
                return result;
            }
            else
            {
                std::cout << "Success " << query << std::endl;
                delete[] query;
                res = new MYSQL_RES;
                res = mysql_store_result(con); // result

                int ret_count = 0, para_count = 0;

                while (ret_count < ret_record)
                {
                    if (!(row = mysql_fetch_row(res)))
                        break;
                    ret_count++;

                    std::string each_res;
                    vector<std::string> each_per_res;
                    para_count = 0; // initial three variable
                    while (para_count < para)
                    {
                        each_per_res.push_back(row[para_count]);
                        each_res = each_res + each_res[para_count];
                        para_count++;
                    }
                    result.push_back(each_res);
                }
                mysql_free_result(res);
            }
            return result; // return true
        }
        else
        {
            std::cout << "**********choose the database fault*************" << std::endl;
            return result; // return null
        }
    }
    else
    {
        std::cout << "unable to connect the database,check your configuration!" << std::endl;
        return result; // return null
    }
}