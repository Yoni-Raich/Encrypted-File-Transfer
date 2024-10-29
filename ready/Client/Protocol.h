#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <tuple>

class Protocol {
public:
    Protocol();
    ~Protocol();

    // Constants
    static constexpr uint32_t MAX_FILE_PACKET_SIZE = 1024;
    static constexpr uint32_t FILE_NAME_SIZE = 255;
    static constexpr uint32_t FILE_HEADER_SIZE = 12;
    static constexpr uint32_t FILE_PACKET_SIZE = MAX_FILE_PACKET_SIZE + FILE_HEADER_SIZE + FILE_NAME_SIZE;

    static constexpr size_t UUID_SIZE = 16;
    static constexpr size_t CLIENT_ID_SIZE = 16;
    static constexpr uint8_t VERSION = 1;
    static constexpr size_t CODE_SIZE = 2;
    static constexpr size_t PAYLOAD_SIZE_SIZE = 4;
    static constexpr size_t MAX_NAME_LENGTH = 255;
    static constexpr size_t MAX_PUBLIC_KEY_SIZE = 160;

    // Request codes
    static constexpr uint16_t REGISTER_CODE = 825;
    static constexpr uint16_t SEND_PUBLIC_KEY_CODE = 826;
    static constexpr uint16_t RECONNECT_CODE = 827;
    static constexpr uint16_t SEND_FILE_CODE = 828;
    static constexpr uint16_t CRC_OK_CODE = 900;
    static constexpr uint16_t CRC_RETRY_CODE = 901;
    static constexpr uint16_t CRC_FAIL_CODE = 902;

    // Response codes
    static constexpr uint16_t REGISTER_SUCCESS_CODE = 1600;
    static constexpr uint16_t REGISTER_FAILURE_CODE = 1601;
    static constexpr uint16_t KEY_EXCHANGE_SUCCESS_CODE = 1602;
    static constexpr uint16_t FILE_RECEIVED_SUCCESS_CODE = 1603;
    static constexpr uint16_t GENERAL_SUCCESS_CODE = 1604;
    static constexpr uint16_t RECONNECT_SUCCESS_CODE = 1605;
    static constexpr uint16_t KEY_EXCHANGE_FAILURE_CODE = 1606;

    static constexpr size_t MAX_RETRY_ATTEMPTS = 3;

    // Error messages
    const std::string SERVER_RESPOND_ERROR = "server responded with an error";
    const std::string FATAL_REGISTER_ERROR = "Fatal error: Failed to register, ";
    const std::string FATAL_KEY_EXCHANGE_ERROR = "Fatal error: Failed to exchange keys, ";
    const std::string FATAL_CRC_ERROR = "Fatal error: CRC mismatch after 3 attempts";

    // Function declarations
    std::tuple<uint8_t, uint16_t, std::vector<uint8_t>> parse_response(const std::vector<uint8_t>& data);
    std::vector<uint8_t> create_request(uint16_t code, const std::vector<uint8_t>& client_id, uint8_t version, const std::vector<uint8_t>& payload);
    std::vector<uint8_t> create_register_request(const std::vector<uint8_t>& client_id, const std::string& name);
    std::vector<uint8_t> create_public_key_request(const std::vector<uint8_t>& client_id, const std::vector<uint8_t>& public_key);
    std::vector<uint8_t> create_file_request(const std::vector<uint8_t>& client_id, const std::string& filename, uint32_t original_file_size, const std::vector<uint8_t>& file_content, uint16_t packet_num);
    std::vector<uint8_t> create_crc_request(uint16_t code, const std::vector<uint8_t>& client_id, const std::string& filename);
    std::vector<uint8_t> create_reconnect_request(const std::vector<uint8_t>& client_id, const std::vector<uint8_t>& name);
};
