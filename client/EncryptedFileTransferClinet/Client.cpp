#include "Client.h"
#include <iostream>
#include <fstream>
#include <iomanip>

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

        if (!perform_key_exchange()) {
            std::cerr << "Failed to perform key exchange" << std::endl;
            return;
        }

       /* while (true) {
            handle_server_response();
        }*/
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    m_network_manager.disconnect();
}

std::string bytesToUUIDString(const std::vector<uint8_t>& bytes) {
    if (bytes.size() != 16) {
        throw std::invalid_argument("UUID must be 16 bytes");
    }

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < bytes.size(); ++i) {
        ss << std::setw(2) << static_cast<int>(bytes[i]);
        if (i == 3 || i == 5 || i == 7 || i == 9) {
            ss << "-";
        }
    }
    return ss.str();
}

std::vector<uint8_t> stringToBinary(const std::string& input, size_t desiredSize) {
    std::vector<uint8_t> binaryData(desiredSize, 0);  // Initialize with zeros

    size_t inputSize = std::min(input.length(), desiredSize);
    
    for (size_t i = 0; i < inputSize; ++i) {
        binaryData[i] = static_cast<uint8_t>(input[i]);
    }

    return binaryData;
}

std::vector<uint8_t> uuidStringToBinary(const std::string& uuid) {
    std::vector<uint8_t> binaryData;
    binaryData.reserve(16);

    // Remove hyphens from the UUID string
    std::string cleanUuid = uuid;
    cleanUuid.erase(std::remove(cleanUuid.begin(), cleanUuid.end(), '-'), cleanUuid.end());

    // Convert hex string to bytes
    for (size_t i = 0; i < 32; i += 2) {
        std::string byteString = cleanUuid.substr(i, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
        binaryData.push_back(byte);
    }

    return binaryData;
}

bool Client::register_to_server() {
    std::vector<uint8_t> request = m_protocol.create_register_request(stringToBinary(m_client_id, 16), m_name);
    m_network_manager.sendRequest(request);
    
    std::vector<uint8_t> response = m_network_manager.receiveResponse();
    uint8_t version;
    uint16_t code;
    std::vector<uint8_t> payload;
    std::tie(version, code, payload) = m_protocol.parse_response(response);

    if (code == 1600 && !payload.empty()) {
        m_client_id = bytesToUUIDString(payload);
        return true;
    }
    return false;
}
//
bool Client::perform_key_exchange() {
    m_crypto_manager.generateRSAKeys();
    std::vector<uint8_t> public_key = m_crypto_manager.getPublicKey();
    std::vector<uint8_t> nameBin = stringToBinary(m_name, 255);
    nameBin.insert(nameBin.end(), public_key.begin(), public_key.end());
    std::vector<uint8_t> request = m_protocol.create_public_key_request(uuidStringToBinary(m_client_id), nameBin);
    m_network_manager.sendRequest(request);

    std::vector<uint8_t> response = m_network_manager.receiveResponse();
    uint8_t version;
    uint16_t code;
    std::vector<uint8_t> payload;
    std::tie(version, code, payload) = m_protocol.parse_response(response);

    if (code == 1602) {
        // Extract Client ID (first 16 bytes)
        std::vector<uint8_t> client_id_bytes(payload.begin(), payload.begin() + 16);
        std::string received_client_id = bytesToUUIDString(client_id_bytes);

        // Verify that the received Client ID matches our Client ID
        if (received_client_id != m_client_id) {
            std::cerr << "Received Client ID does not match our Client ID" << std::endl;
            return false;
        }

        // Extract encrypted AES key (remaining bytes)
        std::vector<uint8_t> encrypted_aes_key(payload.begin() + 16, payload.end());
        
        // Decrypt the AES key
        std::string aes_key = m_crypto_manager.decryptRSA(encrypted_aes_key);
        m_crypto_manager.setAESKey(aes_key);
        return true;
    }
    return false;
}

//void Client::handle_server_response() {
//    std::vector<uint8_t> response = m_network_manager.receiveDataBytes();
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