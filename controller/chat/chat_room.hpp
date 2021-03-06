#pragma once

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <memory>
#include <set>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "./chat_message.hpp"
#include "../participants.hpp"

class chat_room
{
private:
    boost::asio::io_service::strand m_strand;
    std::mutex mtx;
    std::set<chat_participant_ptr> participants;
    enum
    {
        max_recent_msgs = 100
    };
    chat_message_queue recent_msgs_;

public:
    chat_room(boost::asio::io_service &ios);
    void join(chat_participant_ptr participant);
    void leave(chat_participant_ptr participant);
    void deliever(const std::string &userName, const std::string &send_info);
    bool is_participant_empty();
};

chat_room::chat_room(boost::asio::io_service &ios)
                     : m_strand(ios) {}

void chat_room::join(chat_participant_ptr participant)
{
    this->m_strand.dispatch([this, participant]() {
        participants.insert(participant);
    });

    std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
                  boost::bind(&chat_participant::deliver, participant, _1));
}

void chat_room::leave(chat_participant_ptr participant)
{
    this->mtx.lock();
    participants.erase(participant);
    this->mtx.unlock();
}

void chat_room::deliever(const std::string &userName, const std::string &send_info)
{
    deliver_msg msg(userName, send_info);
    this->m_strand.dispatch([this, &userName, &send_info, &msg]() {
        recent_msgs_.push_back(msg);
        while (recent_msgs_.size() > max_recent_msgs)
            recent_msgs_.pop_front();
    });

    std::for_each(participants.begin(), participants.end(),
                  boost::bind(&chat_participant::deliver, _1, boost::ref(msg)));
}

bool chat_room::is_participant_empty()
{
    return this->participants.empty();
}

typedef std::shared_ptr<chat_room> chat_room_ptr;