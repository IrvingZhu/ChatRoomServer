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
    std::set<chat_participant_ptr> participants;
    enum
    {
        max_recent_msgs = 100
    };
    chat_message_queue recent_msgs_;

public:
    void join(chat_participant_ptr participant);
    void leave(chat_participant_ptr participant);
    void deliever(const std::string &userName, const std::string &send_info);
};

void chat_room::join(chat_participant_ptr participant)
{
    participants.insert(participant);
    std::for_each(recent_msgs_.begin(), recent_msgs_.end(),
                  boost::bind(&chat_participant::deliver, participant, _1));
}

void chat_room::leave(chat_participant_ptr participant)
{
    participants.erase(participant);
}

void chat_room::deliever(const std::string &userName, const std::string &send_info)
{
    chat_message msg(userName, send_info);

    recent_msgs_.push_back(msg);
    while (recent_msgs_.size() > max_recent_msgs)
        recent_msgs_.pop_front();

    std::for_each(participants.begin(), participants.end(),
                  boost::bind(&chat_participant::deliver, _1, boost::ref(msg)));
}

typedef std::shared_ptr<chat_room> chat_room_ptr;