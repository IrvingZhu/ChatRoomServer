#pragma once

#include <iostream>
#include <memory>

#define WIN32_LEAN_AND_MEAN

#include "../utility/retriveData/retriveData.hpp"
#include "../utility/convert/convert.hpp"
#include "./chat/chat_controller.hpp"
#include "./chat/chat_message.hpp"
#include "./read_handler/login.hpp"
#include "./read_handler/register.hpp"
#include "./read_handler/userabout.hpp"
#include "./read_handler/modityabout.hpp"
#include "./read_handler/createroom.hpp"
#include "./read_handler/joinroom.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>

using namespace std;

int login_info = 2, register_info = 2, modify_about_info = 2, create_info = 3, join_info = 3, access_info = 2, chat_info = 3, user_about_info = 1;

typedef std::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr;
typedef std::map<std::string, chat_server_ptr> chat_server_map;

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
                                                   acceptor(ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8902)),
                                                   status(0)
    {
        memset(this->buffer, 0, strlen(buffer));
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
        {
            cout << ec.message() << '\n';
            return;
        }
        cout << "client: " << sock->remote_endpoint().address() << endl;
        sock->async_read_some(boost::asio::buffer(this->buffer), boost::bind(&server::read_handler, this, boost::asio::placeholders::error, sock));
        start(); //retry to accept the next quest ......
    }

    void read_handler(const boost::system::error_code &ec, sock_ptr sock)
    {
        // get buffer information
        int init_pos = 0;
        std::string comBuffer(this->buffer);
        cout << "the receive buffer's content is : " << comBuffer << "\n";

        // reset information
        memset(this->buffer, 0, strlen(this->buffer));

        // find command and info
        auto posi = comBuffer.find(" ");
        if (posi > 1024)
        {
            sock->async_write_some(boost::asio::buffer("InfoError/"), boost::bind(&server::start, this));
            return;
        }
        auto command = comBuffer.substr(init_pos, posi - init_pos);
        auto content = comBuffer.substr(posi + 1);

        // main work logic
        if (command.compare("Login") == 0)
        {
            Login(content, login_info, sock);
        }
        else if (command.compare("Register") == 0)
        {
            Register_user(content, login_info, sock);
        }
        else if (command.compare("SearchUserAllInfo") == 0 || command.compare("SearchUserAllJoinedRoom") == 0)
        {
            userAbout(content, command, user_about_info, sock);
        }
        else if (command.compare("Modify") == 0 || command.compare("ModPsw") == 0)
        {
            modifyAbout(content, command, modify_about_info, sock);
        }
        else if (command.compare("CreateChatRoom") == 0)
        {
            createRoom(content, create_info, sock);
        }
        // long connect part
        else if (command.compare("JoinNewChatRoom") == 0)
        {
            joinToRoom(content, join_info, sock);
        }
        else if (command.compare("AccessChatRoom") == 0)
        {
            // format is "AccessChatRoom [UserName] [ChatName]"
            auto info_res = retriveData(content, access_info);
            auto iter = servers.find(info_res[1]);

            if (iter == servers.end())
            {
                chat_server_ptr server = std::make_shared<chat_server>();
                servers.insert(pair<string, chat_server_ptr>(info_res[1], server));
                server->join(sock);
            }
            else
            {
                iter->second->join(sock);
            }
            
            this->status = 1;
        }
        else if (command.compare("Chat") == 0 && this->status == 1)
        {
            // info_res format
            // format: "Chat [ChatRoom] [UserName] [Info]"
            // [Info] = 4 byte length info + 32 byte of chat userName + 1024 byte of chat words.
            auto info_res = retriveData(content, chat_info);
            auto iter = servers.find(info_res[0]); // chat room name
            auto this_server = iter->second;

            // size jduge is client things
            this_server->send(sock, info_res[1], info_res[2]);
            boost::system::error_code ec;
            this->accept_handler(ec, sock);
        }
        else
        {
            sock->async_write_some(boost::asio::buffer("InfoError/"), boost::bind(&server::start, this));
        }
    }
};

typedef std::shared_ptr<server> main_server_ptr;

// boost::bind(&func,para1,para2,para3,...)
// in this,para is a sequence of func's parameter.
// but you want to set a placeholder,you can use
// _n to set them,which n is sequence of that after
// bind,and '_n' in what place,it replace that place's
// parameter.