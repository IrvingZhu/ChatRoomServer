#pragma once

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "../../utility/retriveData/retriveData.hpp"
#include "../../dataBase/modPersonalInfo.hpp"
#include "../../utility/stringHandle/stringHandle.hpp"

typedef std::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr;

void modifyAbout(std::string content, std::string command, int info, sock_ptr sock)
{
    if (command.compare("Modify") == 0)
    {
        // format is "Modify [Uid] [Uname]"
        auto info_res = retriveData(content, info);
        auto search_res = modifyPersonalInformation(info_res[0], info_res[1], 0);
        if (search_res == 1)
        {
            sock->async_write_some(boost::asio::buffer("SuccessModify/"), boost::bind(&print_returnInfo, "Success modify user's PeopleInfo"));
        }
        else
        {
            sock->async_write_some(boost::asio::buffer("ErrorModify/"), boost::bind(&print_returnInfo, "Error modify user's PeopleInfo"));
        }
    }
    else if (command.compare("ModPsw") == 0)
    {
        // format is "ModPsw [Uid] [password]"
        auto info_res = retriveData(content, info);
        auto search_res = modifyPersonalInformation(info_res[0], info_res[1], 1);

        if (search_res == 1)
        {
            sock->async_write_some(boost::asio::buffer("SuccessModPsw/"), boost::bind(&print_returnInfo, "Success modify user's PassWord"));
        }
        else
        {
            sock->async_write_some(boost::asio::buffer("ErrorModPsw/"), boost::bind(&print_returnInfo, "Error modify user's PassWord"));
        }
    }
}