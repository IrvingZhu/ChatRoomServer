#pragma once

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "../../utility/retriveData/retriveData.hpp"
#include "../../utility/stringHandle/stringHandle.hpp"
#include "../../dataBase/searchLogin.hpp"

typedef std::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr;


void Login(std::string content, int login_info, sock_ptr sock)
{
    // format is "Login [uname] [upassword]"
    auto info_res = retriveData(content, login_info);
    auto search_res = searchLogin(info_res[0], info_res[1]);

    if (search_res == 1)
    {
        sock->async_write_some(boost::asio::buffer("SuccessLogin/"), boost::bind(&print_returnInfo, "SuccessLogin"));
    }
    else
    {
        sock->async_write_some(boost::asio::buffer("ErrorLogin/"), boost::bind(&print_returnInfo, "ErrorLogin"));
    }
}