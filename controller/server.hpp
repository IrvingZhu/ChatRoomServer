#pragma once

#include <iostream>
#include <memory>

#include "./session.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>

#define WIN32_LEAN_AND_MEAN

using namespace std;

typedef std::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr;
typedef std::map<std::string, chat_room_ptr> chat_room_map;
typedef std::shared_ptr<chat_room_map> map_ptr;

class server
{
private:
    boost::asio::io_service &ios;
    boost::asio::ip::tcp::acceptor acceptor;
    map_ptr mptr;

public:
    server(boost::asio::io_service &io_service);
    ~server();
    void start();
    void accept_handler(chat_session_ptr session, const boost::system::error_code &ec);
};

// server

server::server(boost::asio::io_service &io_service) : ios(io_service),
                                                      acceptor(ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8902)),
                                                      mptr(new chat_room_map())
{
    start();
}

server::~server() {}

void server::start()
{
    chat_session_ptr new_session = std::make_shared<chat_session>(this->ios, this->mptr);
    this->acceptor.async_accept(new_session->socket(),
                                boost::bind(&server::accept_handler, this, new_session,
                                            boost::asio::placeholders::error));
}

void server::accept_handler(chat_session_ptr session, const boost::system::error_code &ec)
{
    if (ec)
    {
        cout << ec.message() << '\n';
        return;
    }
    cout << "client: " << session->ptr_socket()->remote_endpoint().address() << "\n";
    session->receive();
    start(); //retry to accept the next quest ......
}

typedef std::shared_ptr<server> main_server_ptr;
// boost::bind(&func,para1,para2,para3,...)
// in this,para is a sequence of func's parameter.
// but you want to set a placeholder,you can use
// _n to set them,which n is sequence of that after
// bind,and '_n' in what place,it replace that place's
// parameter.