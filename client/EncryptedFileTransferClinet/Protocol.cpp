#include "Protocol.h"
#include <cstring>
#include <stdexcept>
#include <random>
#include <map>

// Constants for message structure
constexpr size_t CLIENT_ID_SIZE = 16;  // Adjust the value as needed
constexpr size_t VERSION = 1;
constexpr size_t CODE_SIZE = 2;
constexpr size_t PAYLOAD_SIZE_SIZE = 4;

// Define payload sizes for each request code
const std::map<uint16_t, uint32_t> PAYLOAD_SIZES = {
    {825, 255},   // Register
    {826, 160 + 255},   // Send Public Key
    {827, 128},   // Reconnect
    {828, 255},   // Send File
    {900, 4},     // CRC OK
    {901, 4},     // CRC Retry
    {902, 4}      // CRC Fail
};

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

std::vector<uint8_t> Protocol::create_request(uint16_t code, const std::vector<uint8_t> client_id, uint8_t version, const std::vector<uint8_t>& payload) {
    std::vector<uint8_t> request;

    // Add client_id (16 bytes)
    request.insert(request.end(), client_id.begin(), client_id.end());

    // Add version (1 byte)
    request.push_back(version);

    // Add code (2 bytes)
    request.push_back((code >> 8) & 0xFF);
    request.push_back(code & 0xFF);

    // Get the required payload size for this code
    uint32_t required_payload_size;
    try {
        required_payload_size = PAYLOAD_SIZES.at(code);
    } catch (const std::out_of_range& e) {
        throw std::runtime_error("Invalid request code");
    }

    // Add payload size (4 bytes)
    request.push_back((required_payload_size >> 24) & 0xFF);
    request.push_back((required_payload_size >> 16) & 0xFF);
    request.push_back((required_payload_size >> 8) & 0xFF);
    request.push_back(required_payload_size & 0xFF);

    // Add payload
    request.insert(request.end(), payload.begin(), payload.end());

    // Pad the payload to the specified size
    if (payload.size() < required_payload_size) {
        request.insert(request.end(), required_payload_size - payload.size(), 0);
    }

    return request;
}

std::vector<uint8_t> Protocol::create_register_request(const std::vector<uint8_t> client_id, const std::string& name) {
    std::vector<uint8_t> payload(name.begin(), name.end());
    return create_request(825, client_id, 3, payload);
}

std::vector<uint8_t> Protocol::create_public_key_request(const std::vector<uint8_t> client_id, const std::vector<uint8_t>& public_key) {
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