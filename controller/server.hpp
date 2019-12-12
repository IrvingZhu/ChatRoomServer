#pragma once

#include <iostream>
#include <memory>

#define WIN32_LEAN_AND_MEAN

#include "../database/searchLogin.hpp"
#include "../database/searchAll.hpp"
#include "../database/registerUser.hpp"
#include "../database/modPersonalInfo.hpp"
#include "../database/createChatRoom.hpp"
#include "../database/selfDefineQuery.hpp"
#include "../database/createRela.hpp"
#include "../utility/retriveData/retriveData.hpp"
#include "../utility/convert/convert.hpp"
#include "../utility/stringHandle/stringHandle.hpp"
#include "./chat/chat_controller.hpp"
#include "./chat/chat_message.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>

using namespace std;

int login_info = 2, register_info = 3, modify_info = 3, create_info = 3, join_info = 3, access_info = 2, chat_info = 3, leave_info = 2;

typedef std::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr;

class server
{
private:
    boost::asio::io_service &ios;
    boost::asio::ip::tcp::acceptor acceptor;
    string comBuffer;
    chat_server_map servers;
    int status;
    // if status is 0,short connecting.
    // else long connecting to chat.

public:
    explicit server(boost::asio::io_service &io) : ios(io),
                                                   acceptor(ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8888)),
                                                   status(0)
    {
        start();
    }

    ~server() {}

    void start()
    {
        sock_ptr sock(new boost::asio::ip::tcp::socket(ios));
        acceptor.async_accept(*sock, boost::bind(&server::accept_handler, this, boost::asio::placeholders::error, sock));
    }

    void accept_handler(const boost::system::error_code &ec, sock_ptr sock)
    {
        if (ec)
            return;
        cout << "client: ";
        cout << sock->remote_endpoint().address() << endl;
        sock->async_read_some(boost::asio::buffer(comBuffer), boost::bind(&server::read_handler, this, boost::asio::placeholders::error, sock));
        start(); //retry to accept the next quest ......
    }

    void read_handler(const boost::system::error_code &, sock_ptr sock)
    {
        int init_pos = 0;
        auto posi = this->comBuffer.find(" ");
        auto command = this->comBuffer.substr(init_pos, posi - init_pos);
        auto content = this->comBuffer.substr(posi);

        // for all,if return integer,true is return 1,false is 0

        if (command.compare("Login") == 0)
        {
            // format is "Login [uname] [upassword]"
            auto info_res = retriveData(content, login_info);
            wstring uname(convertToWString(info_res[0]));
            wstring upassword(convertToWString(info_res[1]));
            auto search_res = searchLogin(uname, upassword);
            if (search_res == 1)
            {
                sock->async_write_some(boost::asio::buffer("SuccessLogin"), boost::bind(&server::start, this));
                auto search_user_info = searchAllOfPeople(uname, 1);
                auto iter = search_user_info.begin();
                string send_info("PeopleInfo");
                while (iter != search_user_info.end())
                {
                    send_info = send_info + " ";
                    send_info = send_info + *iter;
                    iter++;
                }
                sock->async_write_some(boost::asio::buffer(send_info), boost::bind(&server::start, this));
            }
            // boost::bind(&func,para1,para2,para3,...)
            // in this,para is a sequence of func's parameter.
            // but you want to set a placeholder,you can use
            // _n to set them,which n is sequence of that after
            // bind,and '_n' in what place,it replace that place's
            // parameter.
            else
            {
                sock->async_write_some(boost::asio::buffer("ErrorLogin"), boost::bind(&server::start, this));
            }
        }
        else if (command.compare("Register") == 0)
        {
            // format is "Register [uid] [uname] [upassword]"
            auto info_res = retriveData(content, register_info);
            wstring uid(convertToWString(info_res[0]));
            wstring uname(convertToWString(info_res[1]));
            wstring upassword(convertToWString(info_res[2]));
            auto search_res = registerUser(uid, uname, upassword);
            if (search_res == 1)
            {
                sock->async_write_some(boost::asio::buffer("SuccessRegister"), boost::bind(&server::start, this));
            }
            else
            {
                sock->async_write_some(boost::asio::buffer("ErrorRegister"), boost::bind(&server::start, this));
            }
        }
        else if (command.compare("Modify") == 0)
        {
            // format is "Modify [newUid] [newUname] [newPsw]"
            auto info_res = retriveData(content, modify_info);
            wstring uid(convertToWString(info_res[0]));
            wstring uname(convertToWString(info_res[1]));
            wstring upassword(convertToWString(info_res[2]));
            auto search_res = modifyPersonalInformation(uid, uname, upassword);
            if (search_res == 1)
            {
                sock->async_write_some(boost::asio::buffer("SuccessModify"), boost::bind(&server::start, this));
            }
            else
            {
                sock->async_write_some(boost::asio::buffer("ErrorModify"), boost::bind(&server::start, this));
            }
        }
        else if (command.compare("ModPsw") == 0)
        {
            // format is "ModPsw [Uid] [Uname] [password]"
            auto info_res = retriveData(content, modify_info);
            wstring uid(convertToWString(info_res[0]));
            wstring uname(convertToWString(info_res[1]));
            wstring upassword(convertToWString(info_res[2]));
            auto search_res = modifyPersonalInformation(uid, uname, upassword);
            if (search_res == 1)
            {
                sock->async_write_some(boost::asio::buffer("SuccessModPsw"), boost::bind(&server::start, this));
            }
            else
            {
                sock->async_write_some(boost::asio::buffer("ErrorModPsw"), boost::bind(&server::start, this));
            }
        }
        else if (command.compare("CreateChatRoom") == 0)
        {
            // format is "Create [UID] [UName] [RoomName]"
            auto info_res = retriveData(content, create_info);
            wstring uid(convertToWString(info_res[0]));
            wstring uname(convertToWString(info_res[1]));
            wstring roomname(convertToWString(info_res[2]));

            // first,query rela table to find last id.
            wchar_t *rela_query = new wchar_t[64];
            memset(rela_query, 0, wcslen(rela_query));
            wcscpy(rela_query, L"select id from peo_chat_r order by id desc");
            auto search_res = selfDefineQuery(rela_query, 1, 1);

            auto rela_id_res = getNextKey(search_res[0]);

            // second,query chatroomset to find last chatid.
            wchar_t *room_query = new wchar_t[64];
            memset(room_query, 0, wcslen(room_query));
            wcscpy(rela_query, L"select ChatRID from chat_room_set order by ChatRID desc");
            search_res = selfDefineQuery(rela_query, 1, 1);

            auto room_id_res = getNextKey(search_res[0]);

            // thrid,add infomation into database
            auto Wroom_id_res = convertToWString(room_id_res);
            auto Wrela_id_res = convertToWString(rela_id_res);
            int ret_res_room = createChatRoom(Wroom_id_res, roomname);
            int ret_res_rela = createRela(Wrela_id_res, uid, Wroom_id_res);

            if (ret_res_room > 0 && ret_res_rela > 0)
            {
                // create success and join in.
                sock->async_write_some(boost::asio::buffer("SuccessCre"), boost::bind(&server::accept_handler, this, boost::asio::placeholders::error, sock));
                chat_server_ptr server(new chat_server(sock, convertToString(uname)));
                servers.insert(pair<string, chat_server_ptr>(info_res[1], server));
                this->status = 1; //chat begin.
                this->start();
            }
            else
            {
                sock->async_write_some(boost::asio::buffer("ErrorCre"), boost::bind(&server::start, this));
            }
        }
        // long connect part
        else if (command.compare("JoinNewChatRoom") == 0)
        {
            // format is "JoinNewChatRoom [UID] [Uname] [ChatName]"
            auto info_res = retriveData(content, join_info);
            wstring UID(convertToWString(info_res[0]));
            wstring UName(convertToWString(info_res[1]));
            wstring ChatName(convertToWString(info_res[2]));

            wchar_t *wquery = new wchar_t[64];
            memset(wquery, 0, wcslen(wquery));
            swprintf(wquery, L"select ChatID from chatroomset where ChatName = '%s'", ChatName.c_str()); // find ChatRoom in database
            auto search_res = selfDefineQuery(wquery, 1, 1);
            auto ChatID = convertToWString(search_res[0]); // chatid

            if (!search_res.empty())
            {
                // 1.search id desc from rela table
                wchar_t *rela_query = new wchar_t[64];
                memset(rela_query, 0, wcslen(rela_query));
                wcscpy(rela_query, L"select id from peo_chat_r order by id desc");
                auto search_res = selfDefineQuery(rela_query, 1, 1);

                // 2.add this relation into rela table
                auto Rela_ID_S = getNextKey(search_res[0]);
                auto Rela_ID = convertToWString(Rela_ID_S);
                int cre_res = createRela(Rela_ID, UID, ChatID);

                // 3.jduge whether it is success.
                if (cre_res == 1)
                {
                    sock->async_write_some(boost::asio::buffer("SuccessJoin"), boost::bind(&server::accept_handler, this, boost::asio::placeholders::error, sock));
                    chat_server_ptr server(new chat_server(sock, convertToString(UName)));
                    servers.insert(pair<string, chat_server_ptr>(info_res[1], server));
                    this->status = 1;
                    this->start();
                }
                else
                {
                    sock->async_write_some(boost::asio::buffer("ErrorQuery"), boost::bind(&server::start, this));
                }
            }
            else
            {
                sock->async_write_some(boost::asio::buffer("ChatRoomNotExist"), boost::bind(&server::start, this));
            }
        }
        else if (command.compare("AccessChatRoom") == 0)
        {
            // format is "AccessChatRoom [UserName] [ChatName]"
            auto info_res = retriveData(content, access_info);
            auto iter = servers.find(info_res[1]);
            if (iter == servers.end())
            {
                sock->async_write_some(boost::asio::buffer("NotFindRoom"), boost::bind(&server::start, this));
            }
            chat_server_ptr server(new chat_server(sock, info_res[0]));
            servers.insert(pair<string, chat_server_ptr>(iter->first, server));
            sock->async_write_some(boost::asio::buffer("SuccessAccess"), boost::bind(&server::accept_handler, this, boost::asio::placeholders::error, sock));
            this->status = 1;
            this->start();
        }
        else if (command.compare("Chat") == 0 && this->status == 1)
        {
            // info_res format
            // format: "Chat [ChatRoom] [UserName] [Info]"
            // [Info] = 4 byte length info + 32 byte of chat userName + 1024 byte of chat words.
            auto info_res = retriveData(content, chat_info);
            auto iter = servers.find(info_res[0]); // chat room name
            auto this_server = iter->second;
            auto chat_content = info_res[2].c_str();
            // size jduge is client things
            this_server->send(info_res[1], info_res[2]);

            // have not send some infomation to client,because it is chat status.
            this->start();
        }
        else if (command.compare("Leave") == 0 && this->status == 1)
        {
            //format: "Leave [UserName] [ChatRoom]"
            auto info_res = retriveData(content, leave_info);
            auto iter = servers.find(info_res[1]);
            auto this_server = iter->second;
            this_server->leave(info_res[0]);

            sock->async_write_some(boost::asio::buffer("SuccessLeave"), boost::bind(&server::start, this));
        }
        else
        {
            sock->async_write_some(boost::asio::buffer("InfoError"), boost::bind(&server::start, this));
        }
    }
};

typedef std::shared_ptr<server> main_server_ptr;