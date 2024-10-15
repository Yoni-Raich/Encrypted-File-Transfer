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
    const int FILE_NAME_SIZE = 255;
    const int FILE_HAEDER_SIZE = 12;
    const int FILE_PACKET_SIZE = MAX_FILE_PACKET_SIZE + FILE_HAEDER_SIZE + FILE_NAME_SIZE;

    const std::string SERVER_RESPOND_ERROR = "server responded with an error";
	const std::string FATAL_REGISTER_ERROR = "Fatal error: Failed to register, ";
	const std::string FATAL_KEY_EXCHANGE_ERROR = "Fatal error: Failed to exchange keys, ";
	const std::string FATAL_CRC_ERROR = "Fatal error: CRC mismatch after 3 attempts";

    // Parse incoming response from server
    std::tuple<uint8_t, uint16_t, std::vector<uint8_t>> parse_response(const std::vector<uint8_t>& data);

    // Create request to send to server
    std::vector<uint8_t> create_request(uint16_t code, const std::vector<uint8_t> client_id, uint8_t version, const std::vector<uint8_t>& payload);

    // Helper functions for specific message types
    std::vector<uint8_t> create_register_request(const std::vector<uint8_t> client_id, const std::string& name);
    std::vector<uint8_t> create_public_key_request(const std::vector<uint8_t> client_id, const std::vector<uint8_t>& public_key);
    std::vector<uint8_t> create_file_request(const std::vector<uint8_t> client_id, const std::string& filename, const size_t original_file_size, const std::vector<uint8_t>& file_content, const size_t packet_num);
    std::vector<uint8_t> create_crc_request(const int code, const std::vector<uint8_t> client_id, const std::string& filename);
    std::vector<uint8_t> create_reconnect_request(const std::vector<uint8_t> client_id, const std::vector<uint8_t>& name);

    // New constants
    static const size_t UUID_SIZE = 16;
    static const size_t MAX_NAME_LENGTH = 255;
    static const uint16_t REGISTER_SUCCESS_CODE = 1600;
    static const uint16_t REGISTER_FAILURE_CODE = 1601;
    static const uint16_t KEY_EXCHANGE_SUCCESS_CODE = 1602;
    static const uint16_t FILE_RECEIVED_SUCCESS_CODE = 1603;
    static const uint16_t GENERAL_SUCCESS_CODE = 1604;
    static const uint16_t RECONNECT_SUCCESS_CODE = 1605;
    static const uint16_t KEY_EXCHANGE_FAILURE_CODE = 1606;
    static const uint16_t CRC_OK_CODE = 900;
    static const uint16_t CRC_MISMATCH_CODE = 902;
    static const size_t MAX_RETRY_ATTEMPTS = 3;

private:
    const size_t CLIENT_ID_SIZE = 16;
    const size_t VERSION = 1;
    const size_t CODE_SIZE = 2;
    const size_t PAYLOAD_SIZE_SIZE = 4;


};
