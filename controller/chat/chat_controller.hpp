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

class chat_participant
{
public:
    virtual ~chat_participant() {}
    virtual void deliver(const chat_message &msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;

class chat_room
{
public:
    void join(chat_participant_ptr participant)
    {
        // for this part,participant will join the chatroom queue
        participants_.insert(participant);
        std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
                      boost::bind(&chat_participant::deliver, participant, _1));
    }

    void leave(chat_participant_ptr participant)
    {
        participants_.erase(participant);
    }

    void deliver(const chat_message &msg)
    {
        //max message is not over 100
        recent_msgs_.push_back(msg);
        while (recent_msgs_.size() > max_recent_msgs)
            recent_msgs_.pop_front();

        std::for_each(participants_.begin(), participants_.end(),
                      boost::bind(&chat_participant::deliver, _1, boost::ref(msg)));
    }

private:
    std::set<chat_participant_ptr> participants_;
    enum
    {
        max_recent_msgs = 100
    };
    chat_message_queue recent_msgs_;
};

class chat_session
    : public chat_participant,
      public std::enable_shared_from_this<chat_session>
{
public:
    chat_session(std::shared_ptr<boost::asio::ip::tcp::socket> &socket, chat_room &room)
        : socket_(socket),
          room_(room)
    {
        this->start();
    }

    std::shared_ptr<boost::asio::ip::tcp::socket> &socket()
    {
        return socket_;
    }

    void start()
    {
        // begin start,receive the message.
        room_.join(std::dynamic_pointer_cast<chat_participant>(shared_from_this()));
        // boost::asio::async_read(socket_,
        //                         boost::asio::buffer(read_msg_.data(), chat_message::header_length),
        //                         boost::bind(
        //                             &chat_session::handle_read_header, shared_from_this(),
        //                             boost::asio::placeholders::error));
        this->socket_->async_read_some(boost::asio::buffer(read_msg_.data(), chat_message::header_length),
                                    boost::bind(
                                    &chat_session::handle_read_header, shared_from_this(),
                                    boost::asio::placeholders::error)                                        
                                    );
    }

    void deliver(const chat_message &msg)
    {
        // before weite_msgs_ empty,push your message to the queue
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress)
        {
            // boost::asio::async_write(socket_,
            //                          boost::asio::buffer(write_msgs_.front().data(),
            //                                              write_msgs_.front().length()),
            //                          boost::bind(&chat_session::handle_write, shared_from_this(),
            //                                      boost::asio::placeholders::error));
            this->socket_->async_write_some(boost::asio::buffer(write_msgs_.front().data(),
                                            write_msgs_.front().length()),
                                            boost::bind(&chat_session::handle_write, shared_from_this(),
                                            boost::asio::placeholders::error));
        }
    }

    void handle_read_header(const boost::system::error_code &error)
    {
        if (!error && read_msg_.decode_header())
        {
            // in this calling,return immediately
            // boost::asio::async_read(socket_,
            //                         boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
            //                         boost::bind(&chat_session::handle_read_body, shared_from_this(),
            //                                     boost::asio::placeholders::error));
            this->socket_->async_read_some(boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
                                           boost::bind(&chat_session::handle_read_body, shared_from_this(),
                                           boost::asio::placeholders::error));
        }
        else
        {
            room_.leave(std::dynamic_pointer_cast<chat_participant>(shared_from_this()));
        }
    }

    void handle_read_body(const boost::system::error_code &error)
    {
        if (!error)
        {
            room_.deliver(read_msg_);
            // in this calling,return immediately
            // boost::asio::async_read(socket_,
            //                         boost::asio::buffer(read_msg_.data(), chat_message::header_length),
            //                         boost::bind(&chat_session::handle_read_header, shared_from_this(),
            //                                     boost::asio::placeholders::error));
            this->socket_->async_read_some(boost::asio::buffer(read_msg_.data(), chat_message::header_length),
                                           boost::bind(&chat_session::handle_read_header, shared_from_this(),
                                           boost::asio::placeholders::error));
        }
        else
        {
            room_.leave(std::dynamic_pointer_cast<chat_participant>(shared_from_this()));
        }
    }

    void handle_write(const boost::system::error_code &error)
    {
        if (!error)
        {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
                // in this calling,return immediately
                // boost::asio::async_write(socket_,
                //                          boost::asio::buffer(write_msgs_.front().data(),
                //                                              write_msgs_.front().length()),
                //                          boost::bind(&chat_session::handle_write, shared_from_this(),
                //                                      boost::asio::placeholders::error));
                this->socket_->async_write_some(boost::asio::buffer(write_msgs_.front().data(),
                                                write_msgs_.front().length()),
                                                boost::bind(&chat_session::handle_write, shared_from_this(),
                                                boost::asio::placeholders::error));
            }
        }
        else
        {
            room_.leave(std::dynamic_pointer_cast<chat_participant>(shared_from_this()));
        }
    }

private:
    std::shared_ptr<boost::asio::ip::tcp::socket> &socket_;
    chat_room &room_;
    chat_message read_msg_;
    chat_message_queue write_msgs_;
};

typedef std::shared_ptr<chat_session> chat_session_ptr;

class chat_server
{
public:
    chat_server(std::shared_ptr<boost::asio::ip::tcp::socket> &sock)
        : sock_(sock)
    {
        start_chat();
    }

    void start_chat() // if you want to add a room of session,you can use this function.
    {
        chat_session_ptr new_session(new chat_session(this->sock_, room_));
    }

private:
    std::shared_ptr<boost::asio::ip::tcp::socket> &sock_;
    chat_room room_;
};

typedef std::shared_ptr<chat_server> chat_server_ptr;
typedef std::map<std::string, chat_server_ptr> chat_server_map;