#pragma once

#include "cksum_new.h"
#include "Protocol.h"
#include "NetworkManager.h"
#include "CryptoManager.h"
#include "FileManager.h"
#include <string>
#include <vector>

class Client {
public:
	Client(const std::string& server_ip, const std::string& server_port, const std::string& username, const std::string& filePath, FileManager& fileManager);
    ~Client();

    void run();

    bool InitClientData();

private:
    std::string m_client_id;
    std::string m_name;
	std::string m_filePath;
    Protocol m_protocol;
    NetworkManager m_network_manager;
    CryptoManager m_crypto_manager;
    FileManager m_fileManager;


    void register_to_server();
    void perform_initial_key_exchange();
    void perform_reconnect();
    void receive_and_process_aes_key();
    void handle_server_response(std::string filePath);
    void send_file(const std::string& filename);
    void handle_crc_response(std::string filePath, const std::vector<uint8_t>& payload);
};