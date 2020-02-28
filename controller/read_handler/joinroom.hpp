#pragma once

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "../../utility/retriveData/retriveData.hpp"
#include "../../dataBase/selfDefineQuery.hpp"
#include "../../dataBase/createRela.hpp"
#include "../../utility/stringHandle/stringHandle.hpp"

typedef std::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr;

void joinToRoom(std::string content, int info, sock_ptr sock)
{
    // format is "JoinNewChatRoom [UID] [Uname] [RoomName]"
    auto info_res = retriveData(content, info);

    char query[64];
    memset(query, 0, strlen(query));

    string s_query("select ChatRID from chatroomset where ChatName = '");
    string symbol("'\n");
    s_query = s_query + info_res[2] + symbol;

    strcpy(query, s_query.c_str());
    auto search_res = selfDefineQuery(query, 1, 1);
    cout << "the res is: " << search_res[0][0] << "\n";
    auto chatRID = search_res[0][0];

    if (!search_res[0].empty())
    {
        // search exist
        string exist_query("select id from peo_chat_r where uid = '");
        exist_query = exist_query + info_res[0] + "' and ChatRID in (select chatRID from chatroomset where ChatName = '" + info_res[2] + "')";
        cout << "the query sentences is: " << exist_query << "\n";

        char rela_query[256];
        memset(rela_query, 0, strlen(rela_query));
        strcpy(rela_query, exist_query.c_str());

        auto result_set = selfDefineQuery(rela_query, 1, 1);
        if (result_set.size() != 0)
        {
            cout << "Exist joined, return SuccessJoin"
                 << "\n";
            sock->async_write_some(boost::asio::buffer("SuccessJoin/"), boost::bind(&print_returnInfo, "SuccessJoin"));
        }
        else
        {
            // 1.search id desc from rela table
            memset(rela_query, 0, strlen(rela_query));
            strcpy(rela_query, "select id from peo_chat_r order by id desc");
            auto search_res = selfDefineQuery(rela_query, 1, 1);

            // 2.add this relation into rela table
            auto Rela_ID_S = getNextKey(search_res[0][0]);
            int cre_res = createRela(Rela_ID_S, info_res[0], chatRID);

            // 3.jduge whether it is success.
            if (cre_res == 1)
            {
                sock->async_write_some(boost::asio::buffer("SuccessJoin/"), boost::bind(&print_returnInfo, "SuccessJoin"));
            }
            else
            {
                sock->async_write_some(boost::asio::buffer("ErrorQuery/"), boost::bind(&print_returnInfo, "ErrorJoin"));
            }
        }
    }
    else
    {
        sock->async_write_some(boost::asio::buffer("ChatRoomNotExist/"), boost::bind(&print_returnInfo, "RoomExist"));
    }
}