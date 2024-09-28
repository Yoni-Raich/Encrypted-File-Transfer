#include "Client.h"
#include <iostream>
#include <fstream>

Client::Client(const std::string& server_ip, const std::string& server_port)
    : m_network_manager(server_ip, server_port) {
    m_client_id = ""; // Generate a unique client ID
    m_name = "ClientName"; // Set a client name
}

Client::~Client() {
}

void Client::run() {
    try {
        m_network_manager.connect();

        if (!register_to_server()) {
            std::cerr << "Failed to register to server" << std::endl;
            return;
        }

        //if (!perform_key_exchange()) {
        //    std::cerr << "Failed to perform key exchange" << std::endl;
        //    return;
        //}

        //while (true) {
        //    handle_server_response();
        //}
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    m_network_manager.disconnect();
}

bool Client::register_to_server() {
   /* std::vector<uint8_t> request = m_protocol.create_register_request(m_client_id, m_name);
    m_network_manager.sendData(request);
    
    std::vector<uint8_t> response = m_network_manager.receiveData();
    uint8_t version;
    uint16_t code;
    std::vector<uint8_t> payload;
    std::tie(version, code, payload) = m_protocol.parse_response(response);

    if (code == 1600 && !payload.empty()) {
        m_client_id = std::string(payload.begin(), payload.begin() + 16);
        return true;
    }*/
    return false;
}
//
//bool Client::perform_key_exchange() {
//    std::vector<uint8_t> public_key = m_crypto_manager.generate_rsa_key_pair();
//    std::vector<uint8_t> request = m_protocol.create_public_key_request(m_client_id, public_key);
//    m_network_manager.sendData(request);
//
//    std::vector<uint8_t> response = m_network_manager.receiveData();
//    uint8_t version;
//    uint16_t code;
//    std::vector<uint8_t> payload;
//    std::tie(version, code, payload) = m_protocol.parse_response(response);
//
//    if (code == 1602) {
//        std::vector<uint8_t> aes_key = m_crypto_manager.decrypt_rsa(payload);
//        m_crypto_manager.set_aes_key(aes_key);
//        return true;
//    }
//    return false;
//}
//
//void Client::handle_server_response() {
//    std::vector<uint8_t> response = m_network_manager.receiveData();
//    uint8_t version;
//    uint16_t code;
//    std::vector<uint8_t> payload;
//    std::tie(version, code, payload) = m_protocol.parse_response(response);
//
//    switch (code) {
//        case 1603: // CRC
//            handle_crc_response(code, payload);
//            break;
//        case 1604: // File transfer approved
//            std::cout << "File transfer approved" << std::endl;
//            break;
//        case 1605: // Reconnect approved
//            std::cout << "Reconnect approved" << std::endl;
//            break;
//        case 1606: // Reconnect denied
//            std::cerr << "Reconnect denied" << std::endl;
//            break;
//        default:
//            std::cerr << "Unknown response code: " << code << std::endl;
//    }
//}
//
//void Client::send_file(const std::string& filename) {
//    std::ifstream file(filename, std::ios::binary);
//    if (!file) {
//        std::cerr << "Failed to open file: " << filename << std::endl;
//        return;
//    }
//
//    std::vector<uint8_t> file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
//    file.close();
//
//    std::vector<uint8_t> encrypted_content = m_crypto_manager.encrypt_aes(file_content);
//    std::vector<uint8_t> request = m_protocol.create_file_request(m_client_id, filename, encrypted_content);
//    m_network_manager.sendData(request);
//}
//
//void Client::handle_crc_response(uint16_t code, const std::vector<uint8_t>& payload) {
//    std::string filename(payload.begin(), payload.begin() + 255);
//    uint32_t crc = *reinterpret_cast<const uint32_t*>(&payload[255]);
//
//    if (m_crypto_manager.verify_crc(filename, crc)) {
//        std::vector<uint8_t> request = m_protocol.create_crc_ok_request(m_client_id, filename);
//        m_network_manager.sendData(request);
//    } else {
//        static int retry_count = 0;
//        if (retry_count < 3) {
//            std::vector<uint8_t> request = m_protocol.create_crc_retry_request(m_client_id, filename);
//            m_network_manager.sendData(request);
//            retry_count++;
//            send_file(filename);
//        } else {
//            std::vector<uint8_t> request = m_protocol.create_crc_fail_request(m_client_id, filename);
//            m_network_manager.sendData(request);
//            retry_count = 0;
//        }
//    }
//}