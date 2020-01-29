#pragma once

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <set>
#include <map>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "./chat_message.hpp"

using boost::asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;
typedef std::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr;

class chat_server : enable_shared_from_this<chat_server>
{
    chat_message_queue this_queue;
    enum
    {
        max_recent_msgs = 100
    };

public:
    chat_server();
    ~chat_server();
    void send(sock_ptr sock, const std::string &UserName, const std::string &send_info);
    void push(const std::string &userName, const std::string &send_info);
    void handle_write(sock_ptr sock, const boost::system::error_code &error, std::deque<chat_message>::iterator &iter);
};

chat_server::chat_server() {}

chat_server::~chat_server() {}

void chat_server::send(sock_ptr sock, const std::string &UserName, const std::string &send_info)
{
    // before weite_msgs_ empty,push your message to the queue
    this->push(UserName, send_info);

    while (this->this_queue.size() > max_recent_msgs)
        this->this_queue.pop_front();

    auto iter = this->this_queue.end();
    sock->async_write_some(boost::asio::buffer(iter->data(),
                                               iter->length()),
                           boost::bind(&chat_server::handle_write, shared_from_this(), sock,
                                       boost::asio::placeholders::error, --iter));
}

void chat_server::push(const std::string &userName, const std::string &send_info)
{
    chat_message msg;
    auto p_info = send_info.c_str();
    msg.encode_user(userName);
    auto sp_info = msg.body();
    strcpy(sp_info, p_info); //construct the send_info.

    this->this_queue.push_back(msg);
}

void chat_server::handle_write(sock_ptr sock, const boost::system::error_code &error, chat_message_queue::iterator &iter)
{
    if (!error)
    {
        if (iter != this->this_queue.begin())
        {
            sock->async_write_some(boost::asio::buffer(iter->data(),
                                                       iter->length()),
                                   boost::bind(&chat_server::handle_write, shared_from_this(), sock,
                                               boost::asio::placeholders::error, --iter));
        }
    }
}

typedef std::shared_ptr<chat_server> chat_server_ptr;