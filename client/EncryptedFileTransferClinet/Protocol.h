#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <tuple>

enum RequestCode {
    REGISTER = 1100,
    LOGIN = 1101,
    KEY_EXCHANGE = 1102,
    FILE_SEND = 1103,
    FILE_RECEIVE = 1104,
    CRC_VALID = 1105,
    CRC_INVALID = 1106,
    CRC_FAILED = 1107
};

class Protocol {
public:
    Protocol();
    ~Protocol();

    std::vector<uint8_t> createRequest(const std::string& client_id, RequestCode code, const std::vector<uint8_t>& payload);
    void parseResponse(const std::vector<uint8_t>& response,
        std::string& client_id,
        uint8_t& version,
        RequestCode& code,
        std::vector<uint8_t>& payload);

    static std::vector<uint8_t> createResponse(uint16_t code, const std::string& client_id, const std::vector<uint8_t>& additional_data);

    static std::tuple<std::string, uint8_t, uint16_t, std::vector<uint8_t>> parseRequest(const std::vector<uint8_t>& data);

private:
    static const uint8_t VERSION = 1;
    static const size_t HEADER_SIZE = 24;
};