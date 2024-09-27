#pragma once

#include <string>
#include "Protocol.h"
#include "CryptoManager.h"
#include "FileHandler.h"
#include "NetworkManager.h"

class Client {
public:
    Client(const std::string server_ip, std::string server_port);
    ~Client();

    void run();

private:
    std::string m_server_ip;
    std::string m_server_port;
    Protocol m_protocol;
    CryptoManager m_crypto_manager;
    FileHandler m_file_handler;
    NetworkManager m_network_manager;
    std::string m_client_id;

    bool registerToServer();
    bool performKeyExchange();
    void handleUserInput();
    void processServerResponse();
};