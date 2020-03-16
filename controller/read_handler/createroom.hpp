#pragma once

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "../../utility/retriveData/retriveData.hpp"
#include "../../dataBase/selfDefineQuery.hpp"
#include "../../dataBase/createChatRoom.hpp"
#include "../../dataBase/createRela.hpp"
#include "../../utility/stringHandle/stringHandle.hpp"

typedef std::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr;

void createRoom(std::string content, int info, sock_ptr sock)
{
    // format is "CreateChatRoom [UID] [UName] [RoomName]"
    auto info_res = retriveData(content, info);

    // search exist
    string exist_query("select id from peo_chat_r where uid = '");
    exist_query = exist_query + info_res[0] + "' and ChatRID in (select chatRID from chatroomset where ChatName = '" + info_res[2] + "')";
    Log("the query sentences is: " + exist_query + "\n", false);

    char *rela_query = new char[256];
    memset(rela_query, 0, 64 * sizeof(char));
    strcpy(rela_query, exist_query.c_str());

    auto result_set = selfDefineQuery(rela_query, 1, 1);
    if (result_set.empty() == false)
    {
        Log("Result : Exist chatroom, cannot to create\n", false);
        sock->async_write_some(boost::asio::buffer("ErrorCre/"), boost::bind(&print_returnInfo, "ErrorToCreate"));
        delete rela_query;
    }
    else
    {
        // first,query rela table to find last id.
        memset(rela_query, 0, strlen(rela_query));
        strcpy(rela_query, "select id from peo_chat_r order by id desc");
        auto search_res = selfDefineQuery(rela_query, 1, 1);

        auto rela_id_res = getNextKey(search_res[0][0]);

        // second,query chatroomset to find last chatid.
        char *room_query = new char[64];
        memset(room_query, 0, strlen(room_query));
        strcpy(room_query, "select ChatRID from chatroomset order by ChatRID desc");
        search_res = selfDefineQuery(room_query, 1, 1);

        auto room_id_res = getNextKey(search_res[0][0]);

        // thrid,add infomation into database
        int ret_res_room = createChatRoom(room_id_res, info_res[2]);
        int ret_res_rela = createRela(rela_id_res, info_res[0], room_id_res);

        if (ret_res_room > 0 && ret_res_rela > 0)
        {
            // create success and join in.
            sock->async_write_some(boost::asio::buffer("SuccessCre/"), boost::bind(&print_returnInfo, "SuccessCreate"));
        }
        else
        {
            sock->async_write_some(boost::asio::buffer("ErrorCre/"), boost::bind(&print_returnInfo, "ErrorToCreate"));
        }

        delete rela_query;
        delete room_query;
    }
}