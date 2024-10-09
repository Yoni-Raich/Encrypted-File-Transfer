#pragma once

#include "cksum_new.h"
#include "Protocol.h"
#include "NetworkManager.h"
#include "CryptoManager.h"
#include <string>
#include <vector>

class Client {
public:
    Client(const std::string& server_ip, const std::string& server_port);
    ~Client();

    void run();

private:
    std::string m_client_id;
    std::string m_name;
    Protocol m_protocol;
    NetworkManager m_network_manager;
    CryptoManager m_crypto_manager;

    bool register_to_server();
    bool perform_key_exchange();
    void handle_server_response(std::string filePath);
    void send_file(const std::string& filename);
    void handle_crc_response(std::string filePath, const std::vector<uint8_t>& payload);
};