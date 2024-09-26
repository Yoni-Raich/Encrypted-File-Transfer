//#include "Client.h"
#include "NetworkManager.h"
#include <iostream>

int main(int argc, char* argv[]) {

    std::string server_ip = "localhost";
    std::string server_port = "1234";

    try {
        NetworkManager network_manager(server_ip, server_port);
        network_manager.connect();
        network_manager.sendData("Hello, World!");
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}