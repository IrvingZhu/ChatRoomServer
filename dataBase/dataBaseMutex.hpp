#pragma once
#include <mutex>

#ifndef DBUTIL
    static std::mutex DBmtx;

    MYSQL *con;
	MYSQL_RES *res;
	MYSQL_ROW row;
	//database configuartion
	char dbip[32] = "localhost";
	char dbuser[32] = "root";
	char dbpasswd[32] = "root";
	char dbname[32] = "chatroom";
	char query[512] = "";

	void clearDBqrConf(){
		memset(query, 0, 512 * sizeof(char));
	}

    #define DBUTIL
#endif


