#include "Client.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include "cksum_new.h"

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
       
        send_file("C:\\final_project\\client\\EncryptedFileTransferClinet\\x64\\Debug\\test.txt");
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

void Client::handle_server_response(std::string filePath) {
    std::vector<uint8_t> response = m_network_manager.receiveResponse();
    uint8_t version;
    uint16_t code;
    std::vector<uint8_t> payload;
    std::tie(version, code, payload) = m_protocol.parse_response(response);

    switch (code) {
        case 1603: // CRC
            handle_crc_response(filePath, payload);
            break;
        case 1604: // File transfer approved
            std::cout << "File transfer approved" << std::endl;
            break;
        case 1605: // Reconnect approved
            std::cout << "Reconnect approved" << std::endl;
            break;
        case 1606: // Reconnect denied
            std::cerr << "Reconnect denied" << std::endl;
            break;
        default:
            std::cerr << "Unknown response code: " << code << std::endl;
    }
}

void Client::send_file(const std::string& filePath) 
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return;
    }
	std::filesystem::path filePathsys = filePath;
	std::string filename = filePathsys.filename().string();
    std::vector<uint8_t> file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    std::vector<uint8_t> encrypted_content = m_crypto_manager.encryptAES(file_content);
    size_t totPackeg = encrypted_content.size() / m_protocol.MAX_FILE_PACKET_SIZE + 1;
    std::vector<uint8_t> bit_client_id = uuidStringToBinary(m_client_id);
    for (size_t i = 1; i <= totPackeg; i++) 
    {
		std::vector<uint8_t> request = m_protocol.create_file_request(bit_client_id, filename, file_content.size(),encrypted_content, i);
        
        i == 1 
            ? m_network_manager.sendRequest(request) 
            : m_network_manager.sebdFilePacket(request, m_protocol.FILE_PACKET_SIZE);
	}

	handle_server_response(filePath);
}

void Client::handle_crc_response(std::string filePath, const std::vector<uint8_t>& payload) {
    std::string filename(payload.begin()+20, payload.begin() + 255);
	//extract the value from the last 4 bytes of the payload
	int payloadSize = payload.size();
    uint32_t crc = (payload[payloadSize - 4] << 24) | (payload[payloadSize - 3] << 16) 
        | (payload[payloadSize - 2] << 8) | payload[payloadSize - 1];
    std::vector<uint8_t> bit_client_id = uuidStringToBinary(m_client_id);
    uint32_t current_crc = readfile(filePath);
	std::cout << "CRC: " << current_crc << std::endl;
    if (current_crc == crc) {
        std::vector<uint8_t> request = m_protocol.create_crc_request(900, bit_client_id, filename);
        m_network_manager.sendRequest(request);
    } else {
        static int retry_count = 0;
        if (retry_count < 3) {
            std::vector<uint8_t> request = m_protocol.create_crc_request(901, bit_client_id, filename);
            m_network_manager.sendRequest(request);
            retry_count++;
            send_file(filename);
        } else {
            std::vector<uint8_t> request = m_protocol.create_crc_request(902, bit_client_id, filename);
            m_network_manager.sendRequest(request);
            retry_count = 0;
        }
    }
}