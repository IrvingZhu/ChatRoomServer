#pragma once

<<<<<<< HEAD
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

// information
=======
#include <cstdio>
#include <cstdlib>
#include <cstring>

// information 
>>>>>>> CRS_Project_Mod
// body+header

class chat_message
{
<<<<<<< HEAD
public:
    enum
    {
        header_length = 4
    };
    enum
    {
        max_user_length = 32
    };
    enum
    {
        max_body_length = 1024
    };

    chat_message()
        : body_length_(0),
          user_length_(0)
    {
    }

    const char *data() const
    {
        return data_;
    }

    char *data()
    {
        return data_;
    }

    size_t length() const
    {
        return header_length + user_length_ + body_length_;
    }

    const char *body() const
    {
        return data_ + header_length + user_length_;
    }

    char *body()
    {
        return data_ + header_length + user_length_;
    }

    size_t body_length() const
    {
        return body_length_;
    }

    void body_length(size_t new_length)
    {
        body_length_ = new_length;
        if (body_length_ > max_body_length)
            body_length_ = max_body_length;
    }

    bool decode_header()
    {
        using namespace std; // For strncat and atoi.
        char header[header_length + 1] = "";
        strncat(header, data_, header_length);
        body_length_ = atoi(header);
        if (body_length_ > max_body_length)
        {
            body_length_ = 0;
            return false;
        }
        return true;
    }

    void encode_header()
    {
        using namespace std; // For sprintf and memcpy.
        char header[header_length + 1] = "";
        sprintf(header, "%4d", body_length_);
        memcpy(data_, header, header_length);
    }

    bool encode_user(string username)
    {
        if (username.length() >= max_user_length)
        {
            return false;
        }
        auto p_user = username.c_str();
        strcpy(data_ + header_length, p_user);
        return true;
    }

private:
    char data_[header_length + max_user_length + max_body_length];
    size_t body_length_; // length of data
    size_t user_length_;
    // in this message format,header is length of
    // information.
=======
    public:
        enum { header_length = 32 };
        enum { max_body_length = 512 };

        chat_message()
            : body_length_(0)
        {
        }

        const char* data() const
        {
            return data_;
        }

        char* data()
        {
            return data_;
        }

        size_t length() const
        {
            return header_length + body_length_;
        }

        const char* body() const
        {
            return data_ + header_length;
        }

        char* body()
        {
            return data_ + header_length;
        }

        size_t body_length() const
        {
            return body_length_;
        }

        void body_length(size_t new_length)
        {
            body_length_ = new_length;
            if (body_length_ > max_body_length)
            body_length_ = max_body_length;
        }

        bool decode_header()
        {
            using namespace std; // For strncat and atoi.
            char header[header_length + 1] = "";
            strncat(header, data_, header_length);
            body_length_ = atoi(header);
            if (body_length_ > max_body_length)
            {
            body_length_ = 0;
            return false;
            }
            return true;
        }

        void encode_header()
        {
            using namespace std; // For sprintf and memcpy.
            char header[header_length + 1] = "";
            sprintf(header, "%4d", body_length_);
            memcpy(data_, header, header_length);
        }

    private:
        char data_[header_length + max_body_length];
        size_t body_length_;
>>>>>>> CRS_Project_Mod
};