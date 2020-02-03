#include <iostream>
#include <csignal>

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

        io_service.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    // system("pause");
    return 0;
}