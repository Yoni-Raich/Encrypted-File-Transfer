//#include "Client.h"
#include "NetworkManager.h"
#include "CryptoManager.h"
#include <iostream>
#include "Client.h"


int main(int argc, char* argv[]) {

    std::string server_ip = "localhost";
    std::string server_port = "1234";

    try 
    {
        Client client(server_ip, server_port);
        client.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}