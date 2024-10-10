//#include "Client.h"
#include "NetworkManager.h"
#include "CryptoManager.h"
#include <iostream>
#include "Client.h"
#include "cksum_new.h"
#include "FileManager.h"



int main(int argc, char* argv[]) {

	FileManager fileManager;
    fileManager.readTransferInfo();
    std::string server_ip = fileManager.getIpAddress();
	std::string server_port = fileManager.getPort();
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