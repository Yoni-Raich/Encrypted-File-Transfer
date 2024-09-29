#include "Protocol.h"
#include <cstring>
#include <stdexcept>
#include <random>

// Constants for message structure
constexpr size_t CLIENT_ID_SIZE = 16;  // Adjust the value as needed
constexpr size_t VERSION = 1;
constexpr size_t CODE_SIZE = 2;
constexpr size_t PAYLOAD_SIZE_SIZE = 4;

Protocol::Protocol() {}

Protocol::~Protocol() {}

std::tuple<uint8_t, uint16_t, std::vector<uint8_t>> Protocol::parse_response(const std::vector<uint8_t>& data) {
    if (data.size() < VERSION + CODE_SIZE + PAYLOAD_SIZE_SIZE) {
        throw std::runtime_error("Invalid response size");
    }

    uint8_t version = data[0];
    uint16_t code = (data[1] << 8) | data[2];
    uint32_t payload_size = (data[3] << 24) | (data[4] << 16) | (data[5] << 8) | data[6];

    std::vector<uint8_t> payload(data.begin() + VERSION + CODE_SIZE + PAYLOAD_SIZE_SIZE, data.end());

    return std::make_tuple(version, code, payload);
}

std::vector<uint8_t> Protocol::create_request(uint16_t code, const std::string& client_id, uint8_t version, const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> request;

    // Add client_id (16 bytes)
    std::vector<uint8_t> client_id_bin(16, 0);
    std::memcpy(client_id_bin.data(), client_id.c_str(), std::min(client_id.size(), size_t(16)));
    request.insert(request.end(), client_id_bin.begin(), client_id_bin.end());

    // Add version (1 byte)
    request.push_back(version);

    // Add code (2 bytes)
    request.push_back((code >> 8) & 0xFF);
    request.push_back(code & 0xFF);

    // Add payload size (4 bytes)
    uint32_t payload_size = payload.size();
    request.push_back((payload_size >> 24) & 0xFF);
    request.push_back((payload_size >> 16) & 0xFF);
    request.push_back((payload_size >> 8) & 0xFF);
    request.push_back(payload_size & 0xFF);

    // Add payload
    request.insert(request.end(), payload.begin(), payload.end());

    // Pad the payload to the specified size
    if (payload.size() < payload_size) {
        request.insert(request.end(), payload_size - payload.size(), 0);
    }

    return request;
}

std::vector<uint8_t> Protocol::create_register_request(const std::string& client_id, const std::string& name) {
    std::vector<uint8_t> payload(name.begin(), name.end());
    return create_request(825, client_id, 3, payload);
}

std::vector<uint8_t> Protocol::create_public_key_request(const std::string& client_id, const std::vector<uint8_t>& public_key) {
    return create_request(826, client_id,3, public_key);
}
//
//std::vector<uint8_t> Protocol::create_file_request(const std::string& client_id, const std::string& filename, const std::vector<uint8_t>& file_content) {
//    std::vector<uint8_t> payload;
//    payload.insert(payload.end(), filename.begin(), filename.end());
//    payload.insert(payload.end(), file_content.begin(), file_content.end());
//    return create_request(1027, client_id, payload);
//}
//
//std::vector<uint8_t> Protocol::create_crc_ok_request(const std::string& client_id, const std::string& filename) {
//    std::vector<uint8_t> payload(filename.begin(), filename.end());
//    return create_request(1028, client_id, payload);
//}
//
//std::vector<uint8_t> Protocol::create_crc_retry_request(const std::string& client_id, const std::string& filename) {
//    std::vector<uint8_t> payload(filename.begin(), filename.end());
//    return create_request(1029, client_id, payload);
//}
//
//std::vector<uint8_t> Protocol::create_crc_fail_request(const std::string& client_id, const std::string& filename) {
//    std::vector<uint8_t> payload(filename.begin(), filename.end());
//    return create_request(1030, client_id, payload);
//}
//
//// ... You can add more helper functions for other message types if needed ...