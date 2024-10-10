//#include "Client.h"
#include "NetworkManager.h"
#include "CryptoManager.h"
#include <iostream>
#include "Client.h"
#include "cksum_new.h"
#include "FileManager.h"



int main(int argc, char* argv[]) {

	FileManager fileManager;
    if(!fileManager.readTransferInfo())
    {
		std::cerr << "Failed to read transfer info" << std::endl;
		return 1;
    }
    std::string server_ip = fileManager.getIpAddress();
	std::string server_port = fileManager.getPort();
	std::string username = fileManager.getUsername();
	std::string filePath = fileManager.getFilename();
    try 
    {
		Client client(server_ip, server_port, username, filePath, fileManager);
        client.run();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}