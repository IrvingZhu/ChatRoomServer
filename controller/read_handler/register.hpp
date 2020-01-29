#pragma once

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "../../utility/retriveData/retriveData.hpp"
#include "../../dataBase/selfDefineQuery.hpp"
#include "../../dataBase/registerUser.hpp"
#include "../../utility/stringHandle/stringHandle.hpp"

typedef std::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr;

void Register_user(std::string content, int register_info, sock_ptr sock)
{
    // format is "Register [uname] [upassword]"
    auto info_res = retriveData(content, register_info);

    char people_query[64];
    memset(people_query, 0, strlen(people_query));
    strcpy(people_query, "select uid from people order by uid desc");

    auto uid_res = selfDefineQuery(people_query, 1, 1); // has a fault
    auto uid = getNextKey(uid_res[0][0]);               // has a fault

    auto search_res = registerUser(uid, info_res[0], info_res[1]);

    if (search_res == 1)
    {
        sock->async_write_some(boost::asio::buffer("SuccessRegister/"), boost::bind(&print_returnInfo, "SuccessRegister"));
    }
    else
    {
        sock->async_write_some(boost::asio::buffer("ErrorRegister/"), boost::bind(&print_returnInfo, "ErrorRegister"));
    }
}