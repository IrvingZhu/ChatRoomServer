#pragma once

#include <iostream>
#include <memory>

#define WIN32_LEAN_AND_MEAN

#include "../database/searchLogin.hpp"
#include "../database/searchAllOfUser.hpp"
#include "../database/registerUser.hpp"
#include "../database/modPersonalInfo.hpp"
#include "../database/createChatRoom.hpp"
#include "../database/selfDefineQuery.hpp"
#include "../database/createRela.hpp"
#include "../dataBase/searchUserJoinedRoom.hpp"
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

int login_info = 2, register_info = 2, modify_info = 3, create_info = 3, join_info = 3, access_info = 2, chat_info = 3, leave_info = 2;
int search_all_user_info = 1, search_user_joined_info = 1;

typedef std::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr;

class server
{
private:
    boost::asio::io_service &ios;
    boost::asio::ip::tcp::acceptor acceptor;
    // string comBuffer;
    char buffer[2048];
    chat_server_map servers;
    int status;
    // if status is 0,short connecting.
    // else long connecting to chat.

public:
    explicit server(boost::asio::io_service &io) : ios(io),
                                                   acceptor(ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8888)),
                                                   status(0)
    {
        memset(this->buffer, 0, strlen(buffer));
        start();
    }

    ~server()
    {
    }

    void start()
    {
        sock_ptr sock(new boost::asio::ip::tcp::socket(ios));
        acceptor.async_accept(*sock, boost::bind(&server::accept_handler, this, boost::asio::placeholders::error, sock));
    }

    void accept_handler(const boost::system::error_code &ec, sock_ptr sock)
    {
        if (ec)
        {
            cout << ec.message() << '\n';
            return;
        }
        cout << "client: ";
        cout << sock->remote_endpoint().address() << endl;
        sock->async_read_some(boost::asio::buffer(this->buffer), boost::bind(&server::read_handler, this, boost::asio::placeholders::error, sock));
        start(); //retry to accept the next quest ......
    }

    void read_handler(const boost::system::error_code &ec, sock_ptr sock)
    {
        int init_pos = 0;
        std::string comBuffer(this->buffer);
        cout << comBuffer << "\n"
             << endl;

        memset(this->buffer, 0, strlen(this->buffer));

        wstring output_to_terminal(convertToWString(comBuffer));
        wcout << "command Buffer content is :" << output_to_terminal << '\n';

        auto posi = comBuffer.find(" ");
        cout << "the blank position in the:" << posi << endl; // has no problem 2019.12.18

        if (posi > 1024)
        {
            sock->async_write_some(boost::asio::buffer("InfoError"), boost::bind(&server::start, this));
            return;
        }
        auto command = comBuffer.substr(init_pos, posi - init_pos);
        auto content = comBuffer.substr(posi + 1);

        // for all,if return integer,true is return 1,false is 0

        if (command.compare("Login") == 0)
        {
            // format is "Login [uname] [upassword]"
            auto info_res = retriveData(content, login_info);
            auto search_res = searchLogin(info_res[0], info_res[1]);

            if (search_res == 1)
            {
                sock->async_write_some(boost::asio::buffer("SuccessLogin/"), boost::bind(&server::start, this));
            }
            // boost::bind(&func,para1,para2,para3,...)
            // in this,para is a sequence of func's parameter.
            // but you want to set a placeholder,you can use
            // _n to set them,which n is sequence of that after
            // bind,and '_n' in what place,it replace that place's
            // parameter.
            else
            {
                sock->async_write_some(boost::asio::buffer("ErrorLogin/"), boost::bind(&server::start, this));
            }
        }
        else if (command.compare("Register") == 0)
        {
            // format is "Register [uname] [upassword]"
            auto info_res = retriveData(content, register_info);

            char *people_query = new char[64];
            memset(people_query, 0, strlen(people_query));
            strcpy(people_query, "select uid from people order by uid desc");

            auto uid_res = selfDefineQuery(people_query, 1, 1); // has a fault
            auto uid = getNextKey(uid_res[0]);                  // has a fault

            auto search_res = registerUser(uid, info_res[0], info_res[1]);

            if (search_res == 1)
            {
                sock->async_write_some(boost::asio::buffer("SuccessRegister/"), boost::bind(&server::start, this));
            }
            else
            {
                sock->async_write_some(boost::asio::buffer("ErrorRegister/"), boost::bind(&server::start, this));
            }

            delete[] people_query;
        }
        else if (command.compare("SearchUserAllInfo") == 0)
        {
            // format is "SendUserInfo [uname]"
            auto info_res = retriveData(content, search_all_user_info);
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

            sock->async_write_some(boost::asio::buffer(send_info), boost::bind(&server::start, this));
        }
        else if (command.compare("SearchUserAllJoinedRoom") == 0){
            // format is "SearchUserAllJoinedRoom [uid]"
            auto info_res = retriveData(content, search_user_joined_info);
            auto search_room_info = searchUserJoinedRoom(info_res[0]);
            auto iter = search_room_info.begin();
            string send_info("RoomSet");
            while(iter != search_room_info.end()){
                send_info = send_info + " ";
                send_info = send_info + *iter;
                iter++;
            }
            send_info = send_info + "/";
            sock->async_write_some(boost::asio::buffer(send_info), boost::bind(&server::start, this));
        }
        else if (command.compare("Modify") == 0)
        {
            // format is "Modify [newUid] [newUname] [newPsw]"
            auto info_res = retriveData(content, modify_info);
            auto search_res = modifyPersonalInformation(info_res[0], info_res[1], info_res[2]);
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
            auto search_res = modifyPersonalInformation(info_res[0], info_res[1], info_res[2]);

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

            // first,query rela table to find last id.
            char *rela_query = new char[64];
            memset(rela_query, 0, strlen(rela_query));
            strcpy(rela_query, "select id from peo_chat_r order by id desc");
            auto search_res = selfDefineQuery(rela_query, 1, 1);

            auto rela_id_res = getNextKey(search_res[0]);

            // second,query chatroomset to find last chatid.
            char *room_query = new char[64];
            memset(room_query, 0, strlen(room_query));
            strcpy(room_query, "select ChatRID from chat_room_set order by ChatRID desc");
            search_res = selfDefineQuery(room_query, 1, 1);

            auto room_id_res = getNextKey(search_res[0]);

            // thrid,add infomation into database
            int ret_res_room = createChatRoom(room_id_res, rela_id_res);
            int ret_res_rela = createRela(rela_id_res, info_res[0], room_id_res);

            if (ret_res_room > 0 && ret_res_rela > 0)
            {
                // create success and join in.
                sock->async_write_some(boost::asio::buffer("SuccessCre"), boost::bind(&server::accept_handler, this, boost::asio::placeholders::error, sock));
                chat_server_ptr server(new chat_server(sock, info_res[1]));
                servers.insert(pair<string, chat_server_ptr>(info_res[1], server));
                this->status = 1; //chat begin.
                this->start();
            }
            else
            {
                sock->async_write_some(boost::asio::buffer("ErrorCre"), boost::bind(&server::start, this));
            }

            delete[] room_query;
            delete[] rela_query;
        }
        // long connect part
        else if (command.compare("JoinNewChatRoom") == 0)
        {
            // format is "JoinNewChatRoom [UID] [Uname] [ChatName]"
            auto info_res = retriveData(content, join_info);

            char *query = new char[64];
            memset(query, 0, strlen(query));

            string s_query("select ChatID from chatroomset where ChatName = '");
            string symbol("'\n");
            s_query = s_query + info_res[2] + symbol;

            strcpy(query, s_query.c_str());
            auto search_res = selfDefineQuery(query, 1, 1);

            if (!search_res.empty())
            {
                // 1.search id desc from rela table
                char *rela_query = new char[64];
                memset(rela_query, 0, strlen(rela_query));
                strcpy(rela_query, "select id from peo_chat_r order by id desc");
                auto search_res = selfDefineQuery(rela_query, 1, 1);

                // 2.add this relation into rela table
                auto Rela_ID_S = getNextKey(search_res[0]);
                int cre_res = createRela(Rela_ID_S, info_res[0], search_res[0]);

                // 3.jduge whether it is success.
                if (cre_res == 1)
                {
                    sock->async_write_some(boost::asio::buffer("SuccessJoin"), boost::bind(&server::accept_handler, this, boost::asio::placeholders::error, sock));
                    chat_server_ptr server(new chat_server(sock, info_res[1]));
                    servers.insert(pair<string, chat_server_ptr>(info_res[1], server));
                    this->status = 1;
                    this->start();
                }
                else
                {
                    sock->async_write_some(boost::asio::buffer("ErrorQuery"), boost::bind(&server::start, this));
                }

                delete[] rela_query;
            }
            else
            {
                sock->async_write_some(boost::asio::buffer("ChatRoomNotExist"), boost::bind(&server::start, this));
            }

            delete[] query;
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