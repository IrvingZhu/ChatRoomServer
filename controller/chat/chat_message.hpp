#pragma once

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <queue>

// information
// body+header

class chat_message
{
private:
    enum
    {
        max_user_length = 64
    };
    enum
    {
        max_body_length = 1024
    };

    char dataBuf[max_user_length + max_body_length];
    size_t body_length; // length of data
    size_t user_length;
    // in this message format,header is length of
    // information.

    const char *body() const
    {
        return this->dataBuf + max_user_length;
    }

    char *body()
    {
        return this->dataBuf + max_user_length;
    }

    void struct_user(std::string username)
    {
        if(username.size() > max_user_length)
        {
            username = username.substr(max_user_length);
        }
        strcpy(this->data(), username.c_str());
    }

    void struct_info(std::string send_info)
    {
        if (send_info.size() > max_body_length) 
        {
            send_info = send_info.substr(max_body_length);
        }
        strcpy(this->body(), send_info.c_str());
    }

public:
    chat_message(const std::string &username, const std::string &send_info) : body_length(send_info.size()),
          user_length(max_user_length) 
    {
        memset(this->dataBuf, 0, (max_user_length + max_body_length)*sizeof(char));

        if(this->body_length > max_body_length)
            this->body_length = max_body_length;
        
        this->struct_user(username);
        this->struct_info(send_info);
    }

    const char *data() const
    {
        return dataBuf;
    }

    char *data()
    {
        return dataBuf;
    }

    size_t length() const
    {
        return user_length + body_length;
    }

    ~chat_message() {}
};

typedef std::deque<chat_message> chat_message_queue;