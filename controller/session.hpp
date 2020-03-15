#include <memory>
#include <map>

#include "../utility/retriveData/retriveData.hpp"
#include "../utility/signalHandler/signalHandler.hpp"
#include "./participants.hpp"
#include "./chat/chat_message.hpp"
#include "./chat/chat_room.hpp"
#include "./read_handler/login.hpp"
#include "./read_handler/register.hpp"
#include "./read_handler/userabout.hpp"
#include "./read_handler/modityabout.hpp"
#include "./read_handler/createroom.hpp"
#include "./read_handler/joinroom.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/system/error_code.hpp>

int login_info = 2, register_info = 2, modify_about_info = 2, create_info = 3,
    join_info = 3, access_info = 2, chat_info = 3, user_about_info = 1, leave_info = 1;

typedef std::shared_ptr<boost::asio::ip::tcp::socket> sock_ptr;
typedef std::map<std::string, chat_room_ptr> chat_room_map;
typedef std::shared_ptr<chat_room_map> map_ptr;

class chat_session
    : public std::enable_shared_from_this<chat_session>,
      public chat_participant
{
private:
    sock_ptr sock;
    char buffer[2048];
    chat_message_queue write_msgs;
    map_ptr mptr;
    std::mutex mtx;
    int status = 0; // for it, tag the chat condition.
    std::string roomname;
    boost::asio::deadline_timer timer;
    boost::asio::io_service::strand m_strand;
    boost::asio::io_service &ios;

    void handle_write(const boost::system::error_code &ec);
    void read_handler();
    void access_room(std::string content, int access_info);
    void chat(std::string content, int chat_info);
    void leave(std::string content, int leave_info);

public:
    chat_session(boost::asio::io_service &io_service, map_ptr mptr);
    boost::asio::ip::tcp::socket &socket();
    sock_ptr ptr_socket();
    void receive();
    void deliver(const chat_message &msg);
    void heart_beating();
    void heart_handler();
    ~chat_session();
};

// session

chat_session::chat_session(boost::asio::io_service &io_service, map_ptr mptr)
    : sock(new boost::asio::ip::tcp::socket(io_service)),
      mptr(mptr),
      roomname(""),
      timer(io_service),
      m_strand(io_service),
      ios(io_service)
{
    memset(this->buffer, 0, 2048 * sizeof(char));
}

boost::asio::ip::tcp::socket &chat_session::socket()
{
    return *shared_from_this()->sock;
}

sock_ptr chat_session::ptr_socket()
{
    return shared_from_this()->sock;
}

void chat_session::receive()
{
    shared_from_this()->sock->async_read_some(boost::asio::buffer(shared_from_this()->buffer),
                                              boost::bind(
                                                  &chat_session::read_handler, shared_from_this()));
}

void chat_session::deliver(const chat_message &msg)
{
    bool write_in_progress = !write_msgs.empty();
    shared_from_this()->mtx.lock();
    write_msgs.push_back(msg);
    shared_from_this()->mtx.unlock();
    if (!write_in_progress)
    {
        shared_from_this()->sock->async_write_some(boost::asio::buffer(write_msgs.front().data(),
                                                                       write_msgs.front().length() + 1),
                                                   boost::bind(&chat_session::handle_write, shared_from_this(),
                                                               boost::asio::placeholders::error));
        Log(shared_from_this()->sock->remote_endpoint().address().to_string() + ":" + to_string(shared_from_this()->sock->remote_endpoint().port())
             + " the write_msg size is: " + to_string(write_msgs.size()) + "(write)\n", false);
    }
}

void chat_session::handle_write(const boost::system::error_code &ec)
{
    if (!ec)
    {
        shared_from_this()->mtx.lock();
        write_msgs.pop_front();
        shared_from_this()->mtx.unlock();
    }
    if (!write_msgs.empty())
    {
        shared_from_this()->sock->async_write_some(boost::asio::buffer(write_msgs.front().data(),
                                                                       write_msgs.front().length() + 1),
                                                   boost::bind(&chat_session::handle_write, shared_from_this(), ec));
        Log(shared_from_this()->sock->remote_endpoint().address().to_string() + ":" + to_string(shared_from_this()->sock->remote_endpoint().port())
             + " the write_msg size is: " + to_string(write_msgs.size()) + "(handle)\n", false);
    }
}

// main logic
void chat_session::read_handler()
{
    // get buffer information
    std::string comBuffer(shared_from_this()->buffer);
    Log("the receive buffer's content is : " + comBuffer + " :end" + "\n", false);

    // reset information
    memset(shared_from_this()->buffer, 0, strlen(shared_from_this()->buffer));

    // find command and info
    auto posi = comBuffer.find(" ");
    if (posi > 1024)
    {
        shared_from_this()->ptr_socket()->async_write_some(boost::asio::buffer("InfoError/"),
                                                           boost::bind(&print_returnInfo, "InfoError"));
    }
    auto command = comBuffer.substr(0, posi - 0);
    auto content = comBuffer.substr(posi + 1);

    // main work logic
    if (command.compare("Login") == 0)
    {
        Login(content, login_info, shared_from_this()->ptr_socket());
    }
    else if (command.compare("Register") == 0)
    {
        Register_user(content, login_info, shared_from_this()->ptr_socket());
    }
    else if (command.compare("SearchUserAllInfo") == 0 || command.compare("SearchUserAllJoinedRoom") == 0)
    {
        userAbout(content, command, user_about_info, shared_from_this()->ptr_socket());
    }
    else if (command.compare("Modify") == 0 || command.compare("ModPsw") == 0)
    {
        modifyAbout(content, command, modify_about_info, shared_from_this()->ptr_socket());
    }
    else if (command.compare("CreateChatRoom") == 0)
    {
        createRoom(content, create_info, shared_from_this()->ptr_socket());
    }
    else if (command.compare("JoinNewChatRoom") == 0)
    {
        joinToRoom(content, join_info, shared_from_this()->ptr_socket());
    }

    // long connect part
    else if (command.compare("AccessChatRoom") == 0)
    {
        shared_from_this()->status = 1;
        shared_from_this()->access_room(content, access_info);
    }
    else if (command.compare("Chat") == 0)
    {
        shared_from_this()->chat(content, chat_info);
        shared_from_this()->receive();
    }
    else if (command.compare("Leave") == 0)
    {
        shared_from_this()->leave(content, leave_info);
        shared_from_this()->status = 0;
    }

    else if (command.compare("Exist") == 0)
    {
        // if in the chat condition, check the heartbeating.
        if (shared_from_this()->status == 1)
        {
            shared_from_this()->timer.expires_at(shared_from_this()->timer.expires_at() + boost::posix_time::minutes(8));
            shared_from_this()->timer.async_wait(boost::bind(&chat_session::heart_beating, shared_from_this()));
        }
    }

    else
    {
        shared_from_this()->ptr_socket()->async_write_some(boost::asio::buffer("InfoError/"),
                                                           boost::bind(&print_returnInfo, "InfoError"));

        if (shared_from_this()->status == 1)
        {
            shared_from_this()->leave(shared_from_this()->roomname, leave_info);
            shared_from_this()->status = 0;
        }
        if (shared_from_this()->sock->is_open())
            shared_from_this()->sock->close();

        shared_from_this()->timer.cancel();
    }
}

void chat_session::access_room(std::string content, int access_info)
{
    // format is "AccessChatRoom [UserName] [ChatName]"
    auto info_res = retriveData(content, access_info);
    auto iter = mptr->find(info_res[1]);
    std::string room = info_res[1];

    shared_from_this()->roomname = shared_from_this()->roomname + room;

    if (iter == mptr->end())
    {
        chat_room_ptr room = std::make_shared<chat_room>(shared_from_this()->ios);
        shared_from_this()->m_strand.dispatch([this, &info_res, &room]() {
            mptr->insert(std::pair<std::string, chat_room_ptr>(info_res[1], room));
        });

        shared_from_this()->ptr_socket()->async_write_some(boost::asio::buffer("\rSuccessAccess\n\0"),
                                                           boost::bind(&chat_session::receive, shared_from_this()));

        room->join(std::dynamic_pointer_cast<chat_participant>(shared_from_this()));
    }
    else
    {
        shared_from_this()->ptr_socket()->async_write_some(boost::asio::buffer("\rSuccessAccess\n\0"),
                                                           boost::bind(&chat_session::receive, shared_from_this()));
        iter->second->join(std::dynamic_pointer_cast<chat_participant>(shared_from_this()));
    }

    // register timer to heartbeating callback
    shared_from_this()->timer.expires_from_now(boost::posix_time::minutes(8));
    shared_from_this()->timer.async_wait(boost::bind(&chat_session::heart_beating, shared_from_this()));
}

void chat_session::chat(std::string content, int chat_info)
{
    // info_res format
    // format: "Chat [ChatRoom] [UserName] [Info]"
    // [Info] = 4 byte length info + 32 byte of chat userName + 1024 byte of chat words.

    // modify the timer event and time.
    shared_from_this()->timer.expires_at(shared_from_this()->timer.expires_at() + boost::posix_time::minutes(8));
    shared_from_this()->timer.async_wait(boost::bind(&chat_session::heart_beating, shared_from_this()));

    auto info_res = retriveData(content, chat_info);
    auto iter = mptr->find(info_res[0]); // chat room name
    auto this_room = iter->second;

    // size jduge is client things
    this_room->deliever(info_res[1], info_res[2]);
}

void chat_session::leave(std::string content, int leave_info)
{
    // format: "Leave [RoomName]"
    auto info_res = retriveData(content, leave_info);
    auto iter = mptr->find(info_res[0]); // chat room name
    auto this_room = iter->second;

    // leave
    this_room->leave(std::dynamic_pointer_cast<chat_participant>(shared_from_this()));
    shared_from_this()->status = 0;

    if (this_room->is_participant_empty())
    {
        // if it has no participant, delete them.
        shared_from_this()->mtx.lock();
        mptr->erase(iter);
        Log("the " + info_res[0] + " Room has destoryed" + "\n", false);
        shared_from_this()->mtx.unlock();
    }

    shared_from_this()->timer.cancel();
}

void chat_session::heart_beating()
{
    shared_from_this()->ptr_socket()->async_write_some(boost::asio::buffer("\rExist?\n"),
                                                       boost::bind(&chat_session::receive, shared_from_this()));

    shared_from_this()->timer.expires_at(shared_from_this()->timer.expires_at() + boost::posix_time::minutes(1));
    shared_from_this()->timer.async_wait(boost::bind(&chat_session::heart_handler, shared_from_this()));
}

void chat_session::heart_handler()
{
    Log("\n-----------------heart_beating_finished-----------------\n", false);
    return;
}

chat_session::~chat_session()
{
    // if client isn't close socket and leaving
    if (this->status == 1)
    {
        this->leave(this->roomname, leave_info);
        this->status = 0;
    }
    if (this->sock->is_open())
        this->sock->close();

    Log("\n**********************The Client has been destroyed**********************\n", false);
}

typedef std::shared_ptr<chat_session> chat_session_ptr;