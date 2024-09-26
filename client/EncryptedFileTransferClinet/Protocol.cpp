#include "Protocol.h"
#include <cstring>
#include <stdexcept>
/*
// Add these constant definitions at the top of the file or in a header file
constexpr size_t CLIENT_ID_LENGTH = 16;  // Adjust the value as needed
constexpr size_t VERSION_LENGTH = 1;
constexpr size_t CODE_LENGTH = 2;
constexpr size_t PAYLOAD_SIZE_LENGTH = 4;

Protocol::Protocol() {}

Protocol::~Protocol() {}

std::tuple<std::string, uint8_t, uint16_t, std::vector<uint8_t>> Protocol::parseRequest(const std::vector<uint8_t>& data) {
    if (data.size() < CLIENT_ID_LENGTH + VERSION_LENGTH + CODE_LENGTH + PAYLOAD_SIZE_LENGTH) {
        throw std::runtime_error("Invalid data size");
    }

    std::string client_id(data.begin(), data.begin() + CLIENT_ID_LENGTH);
    uint8_t version = data[CLIENT_ID_LENGTH];
    uint16_t code = (data[CLIENT_ID_LENGTH + VERSION_LENGTH] << 8) | data[CLIENT_ID_LENGTH + VERSION_LENGTH + 1];
    uint32_t payload_size = (data[CLIENT_ID_LENGTH + VERSION_LENGTH + CODE_LENGTH] << 24) |
                            (data[CLIENT_ID_LENGTH + VERSION_LENGTH + CODE_LENGTH + 1] << 16) |
                            (data[CLIENT_ID_LENGTH + VERSION_LENGTH + CODE_LENGTH + 2] << 8) |
                            data[CLIENT_ID_LENGTH + VERSION_LENGTH + CODE_LENGTH + 3];

    std::vector<uint8_t> payload(data.begin() + CLIENT_ID_LENGTH + VERSION_LENGTH + CODE_LENGTH + PAYLOAD_SIZE_LENGTH, data.end());

    return std::make_tuple(client_id, version, code, payload);
}

std::vector<uint8_t> Protocol::createResponse(uint16_t code, const std::string& client_id, const std::vector<uint8_t>& additional_data) {
    std::vector<uint8_t> response;

    // Add version (1 byte)
    response.push_back(1);

    // Add code (2 bytes)
    response.push_back((code >> 8) & 0xFF);
    response.push_back(code & 0xFF);

    // Add payload size (4 bytes)
    uint32_t payload_size = CLIENT_ID_LENGTH + additional_data.size();
    response.push_back((payload_size >> 24) & 0xFF);
    response.push_back((payload_size >> 16) & 0xFF);
    response.push_back((payload_size >> 8) & 0xFF);
    response.push_back(payload_size & 0xFF);

    // Add client_id (16 bytes)
    response.insert(response.end(), client_id.begin(), client_id.end());
    if (client_id.size() < CLIENT_ID_LENGTH) {
        response.insert(response.end(), CLIENT_ID_LENGTH - client_id.size(), 0);
    }

    // Add additional data
    response.insert(response.end(), additional_data.begin(), additional_data.end());

    return response;
}
*/