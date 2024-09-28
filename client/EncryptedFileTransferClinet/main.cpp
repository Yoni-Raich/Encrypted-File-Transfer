//#include "Client.h"
#include "NetworkManager.h"
#include "CryptoManager.h"
#include <iostream>


int main(int argc, char* argv[]) {

    std::string server_ip = "localhost";
    std::string server_port = "1234";

    try {
        NetworkManager network_manager(server_ip, server_port);
        CryptoManager crypto_manager;
        network_manager.connect();
        crypto_manager.generateRSAKeys();
        std::string public_key = crypto_manager.getPublicKey();
        network_manager.sendData(public_key);
        std::string new_data = crypto_manager.decryptRSA(network_manager.receiveData());
        crypto_manager.setAESKey(new_data);
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}