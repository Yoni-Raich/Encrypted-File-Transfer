//#include "Client.h"
//#include <iostream>
//#include <string> // Added for std::to_string
//
//Client::Client(const std::string server_ip, std::string server_port)
//    : m_server_ip(server_ip), m_server_port(server_port),
//    m_network_manager(server_ip, server_port) {
//}
//
//Client::~Client() {
//}
//
//void Client::run() {
//    try {
//        m_network_manager.connect();
//
//        if (!registerToServer()) {
//            std::cerr << "Failed to register to server" << std::endl;
//            return;
//        }
//
//        if (!performKeyExchange()) {
//            std::cerr << "Failed to perform key exchange" << std::endl;
//            return;
//        }
//
//        while (true) {
//            handleUserInput();
//            processServerResponse();
//        }
//    }
//    catch (const std::exception& e) {
//        std::cerr << "Error: " << e.what() << std::endl;
//    }
//    m_network_manager.disconnect();
//}
//
//bool Client::registerToServer() {
//    std::vector<uint8_t> request = m_protocol.createRequest(m_client_id, RequestCode::REGISTER, {});
//    m_network_manager.sendData(request);
//    
//    std::vector<uint8_t> response = m_network_manager.receiveData();
//    std::string response_client_id;
//    uint8_t version;
//    RequestCode code;
//    std::vector<uint8_t> payload;
//    m_protocol.parseResponse(response, response_client_id, version, code, payload);
//
//    return code == RequestCode::REGISTER && !payload.empty() && payload[0] == 0;
//}
//
//bool Client::performKeyExchange() {
//    // Generate RSA key pair
//    auto [publicKey, privateKey] = m_crypto_manager.generateRSAKeyPair();
//
//    // Send public key to server
//    std::vector<uint8_t> request = m_protocol.createRequest(m_client_id, RequestCode::KEY_EXCHANGE, publicKey);
//    m_network_manager.sendData(request);
//
//    // Receive encrypted AES key from server
//    std::vector<uint8_t> response = m_network_manager.receiveData();
//    std::string response_client_id;
//    uint8_t version;
//    RequestCode code;
//    std::vector<uint8_t> encryptedAESKey;
//    m_protocol.parseResponse(response, response_client_id, version, code, encryptedAESKey);
//
//    if (code != RequestCode::KEY_EXCHANGE) {
//        return false;
//    }
//
//    // Decrypt AES key using private RSA key
//    std::vector<uint8_t> aesKey = m_crypto_manager.decryptRSA(encryptedAESKey, privateKey);
//    m_crypto_manager.setAESKey(aesKey);
//
//    return true;
//}
//
//void Client::handleUserInput() {
//    // Implement user input handling
//}
//
//void Client::processServerResponse() {
//    // Implement server response processing
//}