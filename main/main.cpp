#include <iostream>
#include <csignal>
#include <thread>

#include "../controller/server.hpp"
#include "../utility/signalHandler/signalHandler.hpp"

#include <boost/asio.hpp>

using namespace std;

int main(int argc, char *argv[])
{
    // signal register
    signal(SIGINT, singalHandler);

    // starting server
    std::cout << "starting launch server......" << std::endl;
    try
    {
        boost::asio::io_service io_service;

        main_server_ptr main_server(new server(io_service));

        std::vector<thread> thread_pool;
        for(int i = 0; i < 7; ++i)
            thread_pool.emplace_back([&io_service](){
                io_service.run();
            });

        io_service.run();

        for(auto &v : thread_pool)
            v.join();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}