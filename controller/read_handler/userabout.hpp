#pragma once

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "../../utility/retriveData/retriveData.hpp"
#include "../../dataBase/searchAllOfUser.hpp"
#include "../../dataBase/searchUserJoinedRoom.hpp"
#include "../../utility/stringHandle/stringHandle.hpp"

typedef std::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr;

void userAbout(std::string content, std::string command, int info, sock_ptr sock)
{
    if (command.compare("SearchUserAllInfo") == 0)
    {
        // format is "SendUserInfo [uname]"
        auto info_res = retriveData(content, info);
        auto search_user_info = searchAllOfPeople(info_res[0], 1);
        auto iter = search_user_info.begin();

        string send_info("PeopleInfo");
        while (iter != search_user_info.end())
        {
            send_info = send_info + " ";
            send_info = send_info + *iter;
            iter++;
        }

        send_info = send_info + "/";
        cout << send_info << endl;

        sock->async_write_some(boost::asio::buffer(send_info), boost::bind(&print_returnInfo, "Success search user's PeopleInfo"));
        cout << "send successful" << endl;
    }
    else if (command.compare("SearchUserAllJoinedRoom") == 0)
    {
        // format is "SearchUserAllJoinedRoom [uid]"
        auto info_res = retriveData(content, info);
        auto search_room_info = searchUserJoinedRoom(info_res[0]);
        auto iter = search_room_info.begin();

        string send_info("RoomSet");
        while (iter != search_room_info.end())
        {
            send_info = send_info + " ";
            send_info = send_info + *iter;
            iter++;
        }

        send_info = send_info + "/";
        cout << send_info << endl;

        sock->async_write_some(boost::asio::buffer(send_info), boost::bind(&print_returnInfo, "Success search user's all joined room"));
    }
}