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
private:
    chat_message_queue recent_msg;
    chat_message_queue write_msg;
    enum
    {
        max_recent_msgs = 100
    };

public:
    chat_server();
    ~chat_server();
    void join(sock_ptr sock);
    void send(sock_ptr &sock, const std::string &UserName, const std::string &send_info);

private:
    void push_to_recent(const std::string &userName, const std::string &send_info);
    void push_to_write(const std::string &userName, const std::string &send_info);
    void handle_write(sock_ptr &sock, const boost::system::error_code &error);
    void join_deliever(sock_ptr sock, chat_message_queue::iterator &iter, const boost::system::error_code &error);
};

chat_server::chat_server() {}

chat_server::~chat_server() {}

void chat_server::join(sock_ptr sock)
{
    sock->write_some(boost::asio::buffer("SuccessAccess/"));

    auto iter = this->recent_msg.begin();
    boost::system::error_code ec;

    std::cout << "Next to send all users room info" << endl;
    this->join_deliever(sock, iter, ec);
}

void chat_server::send(sock_ptr &sock, const std::string &UserName, const std::string &send_info)
{
    // before weite_msgs_ empty,push your message to the queue
    this->push_to_recent(UserName, send_info);
    this->push_to_write(UserName, send_info);
    while (!this->write_msg.empty())
    {
        cout << "the send user is: " << UserName << "; the send info is: " << send_info << "\n";
        sock->write_some(boost::asio::buffer(this->write_msg.front().data(),this->write_msg.front().length()));
        this->write_msg.pop_front();
    }
}

void chat_server::push_to_recent(const std::string &userName, const std::string &send_info)
{
    chat_message msg;
    auto p_info = send_info.c_str();
    msg.encode_user(userName);
    auto sp_info = msg.body();
    strcpy(sp_info, p_info); //construct the send_info.

    this->recent_msg.push_back(msg);

    while (this->recent_msg.size() > max_recent_msgs)
        this->recent_msg.pop_front();
}

void chat_server::push_to_write(const std::string &userName, const std::string &send_info)
{
    chat_message msg;
    auto p_info = send_info.c_str();
    msg.encode_user(userName);
    auto sp_info = msg.body();
    strcpy(sp_info, p_info); //construct the send_info.

    this->write_msg.push_back(msg);
}

void chat_server::join_deliever(sock_ptr sock, chat_message_queue::iterator &iter, const boost::system::error_code &error)
{
    if (iter != this->recent_msg.end())
    {
        sock->async_write_some(boost::asio::buffer(iter->data(),
                                                   iter->length()),
                               boost::bind(&chat_server::join_deliever, this, sock, ++iter, error));
    }
}

typedef std::shared_ptr<chat_server> chat_server_ptr;