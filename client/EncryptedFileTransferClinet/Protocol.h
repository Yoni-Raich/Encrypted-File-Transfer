#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <tuple>

class Protocol {
public:
    Protocol();
    ~Protocol();
    const int MAX_FILE_PACKET_SIZE = 1024;
    // Parse incoming response from server
    std::tuple<uint8_t, uint16_t, std::vector<uint8_t>> parse_response(const std::vector<uint8_t>& data);

    // Create request to send to server
    std::vector<uint8_t> create_request(uint16_t code, const std::vector<uint8_t> client_id, uint8_t version, const std::vector<uint8_t>& payload);

    // Helper functions for specific message types
    std::vector<uint8_t> create_register_request(const std::vector<uint8_t> client_id, const std::string& name);
    std::vector<uint8_t> create_public_key_request(const std::vector<uint8_t> client_id, const std::vector<uint8_t>& public_key);
    std::vector<uint8_t> create_file_request(const std::vector<uint8_t> client_id, const std::string& filename, const size_t original_file_size, const std::vector<uint8_t>& file_content, const size_t packet_num);
    std::vector<uint8_t> create_crc_ok_request(const std::string& client_id, const std::string& filename);
    std::vector<uint8_t> create_crc_retry_request(const std::string& client_id, const std::string& filename);
    std::vector<uint8_t> create_crc_fail_request(const std::string& client_id, const std::string& filename);

private:
    const size_t CLIENT_ID_SIZE = 16;
    const size_t VERSION = 1;
    const size_t CODE_SIZE = 2;
    const size_t PAYLOAD_SIZE_SIZE = 4;


};
