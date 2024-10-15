/**
 * @file main.cpp
 * @brief Secure File Transfer Client Application
 * @author Yoni Raich
 * @date [10/15/2024]
 *
 * This application provides secure file transfer between client and server.
 * Features include user registration, secure key exchange, file encryption,
 * chunked transfer, data integrity checks, and reconnection capabilities.
 * It ensures data confidentiality and integrity throughout the transfer process.
 */

//#include "Client.h"
#include "NetworkManager.h"
#include "CryptoManager.h"
#include <iostream>
#include "Client.h"
#include "cksum_new.h"
#include "FileManager.h"

int main(int argc, char* argv[]) {
    std::cout << "=======================================================" << std::endl;
    std::cout << "Secure File Transfer Client" << std::endl;
    std::cout << "Version: 3.0" << std::endl;
    std::cout << "Author: Yoni Raich" << std::endl;
    std::cout << "ID: 318327962" << std::endl;
    std::cout << "Date: [10/15/2024]" << std::endl;
    std::cout << "=======================================================" << std::endl;
    std::cout << std::endl;

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
