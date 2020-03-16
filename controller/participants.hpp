#pragma once
#include "./chat/chat_message.hpp"
#include <memory>

class chat_participant
{
public:
    virtual ~chat_participant() {}
    virtual void deliver(const deliver_msg &msg) = 0;
};

typedef std::shared_ptr<chat_participant> chat_participant_ptr;