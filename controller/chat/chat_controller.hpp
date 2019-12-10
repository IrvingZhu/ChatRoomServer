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
<<<<<<< HEAD
    void join(const std::string &UserName, chat_participant_ptr participant)
    {
        // for this part,participant will join the chatroom queue
        participants_.insert(std::pair<std::string, chat_participant_ptr>(UserName, participant));
=======
    void join(chat_participant_ptr participant)
    {
        // for this part,participant will join the chatroom queue
        participants_.insert(participant);
>>>>>>> CRS_Project_Mod
        std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
                      boost::bind(&chat_participant::deliver, participant, _1));
    }

<<<<<<< HEAD
    void leave(const std::string &UserName)
    {
        auto iter = participants_.find(UserName);
        participants_.erase(iter);
=======
    void leave(chat_participant_ptr participant)
    {
        participants_.erase(participant);
>>>>>>> CRS_Project_Mod
    }

    void deliver(const chat_message &msg)
    {
        //max message is not over 100
        recent_msgs_.push_back(msg);
        while (recent_msgs_.size() > max_recent_msgs)
            recent_msgs_.pop_front();

<<<<<<< HEAD
        for (auto iter = participants_.begin(); iter != participants_.end(); iter++)
        {
            boost::bind(&chat_participant::deliver, _1, boost::ref(msg));
        }
    }

    chat_participant_ptr findSession(const string &UserName)
    {
        return participants_.find(UserName)->second;
    }

private:
    std::map<std::string, chat_participant_ptr> participants_; // has some participants.
=======
        std::for_each(participants_.begin(), participants_.end(),
                      boost::bind(&chat_participant::deliver, _1, boost::ref(msg)));
    }

private:
    std::set<chat_participant_ptr> participants_;
>>>>>>> CRS_Project_Mod
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
<<<<<<< HEAD
    chat_session(std::shared_ptr<boost::asio::ip::tcp::socket> &socket, chat_room &room, const std::string &UserName)
        : socket_(socket),
          room_(room),
          UName(UserName)
    {
        this->start(UserName);
=======
    chat_session(std::shared_ptr<boost::asio::ip::tcp::socket> &socket, chat_room &room)
        : socket_(socket),
          room_(room)
    {
        this->start();
>>>>>>> CRS_Project_Mod
    }

    std::shared_ptr<boost::asio::ip::tcp::socket> &socket()
    {
        return socket_;
    }

<<<<<<< HEAD
    void start(const std::string &UserName)
    {
        // begin start,receive the message.
        room_.join(UserName, std::dynamic_pointer_cast<chat_participant>(shared_from_this()));
=======
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
>>>>>>> CRS_Project_Mod
    }

    void deliver(const chat_message &msg)
    {
        // before weite_msgs_ empty,push your message to the queue
        bool write_in_progress = !write_msgs_.empty();
        write_msgs_.push_back(msg);
        if (!write_in_progress)
        {
<<<<<<< HEAD
            this->socket_->async_write_some(boost::asio::buffer(write_msgs_.front().data(),
                                                                write_msgs_.front().length()),
                                            boost::bind(&chat_session::handle_write, shared_from_this(),
                                                        boost::asio::placeholders::error));
=======
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
>>>>>>> CRS_Project_Mod
        }
    }

    void handle_write(const boost::system::error_code &error)
    {
        if (!error)
        {
            write_msgs_.pop_front();
            if (!write_msgs_.empty())
            {
<<<<<<< HEAD
                this->socket_->async_write_some(boost::asio::buffer(write_msgs_.front().data(),
                                                                    write_msgs_.front().length()),
                                                boost::bind(&chat_session::handle_write, shared_from_this(),
                                                            boost::asio::placeholders::error));
=======
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
>>>>>>> CRS_Project_Mod
            }
        }
        else
        {
<<<<<<< HEAD

            room_.leave(this->UName);
=======
            room_.leave(std::dynamic_pointer_cast<chat_participant>(shared_from_this()));
>>>>>>> CRS_Project_Mod
        }
    }

private:
    std::shared_ptr<boost::asio::ip::tcp::socket> &socket_;
<<<<<<< HEAD
    chat_room &room_; // only this room,belong to room
    std::string UName;
=======
    chat_room &room_;
    chat_message read_msg_;
>>>>>>> CRS_Project_Mod
    chat_message_queue write_msgs_;
};

typedef std::shared_ptr<chat_session> chat_session_ptr;

class chat_server
{
public:
<<<<<<< HEAD
    chat_server(std::shared_ptr<boost::asio::ip::tcp::socket> &sock, const std::string &UserName)
        : sock_(sock)
    {
        start_chat(UserName);
    }

    void start_chat(const std::string &UserName) // if you want to add a room of session,you can use this function.
    {
        chat_session_ptr new_session(new chat_session(this->sock_, room_, UserName));
    }

    void send(const string &UserName, const string &Info)
    {
        chat_message send_info;
        auto p_info = Info.c_str();
        send_info.encode_user(UserName);
        auto sp_info = send_info.body();
        strcpy(sp_info, p_info); //construct the send_info.

        auto participant = std::dynamic_pointer_cast<chat_session>(room_.findSession(UserName));
        participant->deliver(send_info);
    }

    void leave(const string &UserName)
    {
        room_.leave(UserName);
=======
    chat_server(std::shared_ptr<boost::asio::ip::tcp::socket> &sock)
        : sock_(sock)
    {
        start_chat();
    }

    void start_chat() // if you want to add a room of session,you can use this function.
    {
        chat_session_ptr new_session(new chat_session(this->sock_, room_));
>>>>>>> CRS_Project_Mod
    }

private:
    std::shared_ptr<boost::asio::ip::tcp::socket> &sock_;
<<<<<<< HEAD
    chat_room room_; // server has room
};

typedef std::shared_ptr<chat_server> chat_server_ptr;
typedef std::map<std::string, chat_server_ptr> chat_server_map;

// has one room,one server,but can has no limit of session.
=======
    chat_room room_;
};

typedef std::shared_ptr<chat_server> chat_server_ptr;
typedef std::map<std::string, chat_server_ptr> chat_server_map;
>>>>>>> CRS_Project_Mod
