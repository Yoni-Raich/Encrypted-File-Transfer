#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <tuple>

class Protocol {
public:
    Protocol();
    ~Protocol();

    // Parse incoming request
    std::tuple<std::string, uint8_t, uint16_t, std::vector<uint8_t>> parseRequest(const std::vector<uint8_t>& data);

    // Create general request
    std::vector<uint8_t> createRequest(uint16_t code, const std::string& client_id, const std::vector<uint8_t>& payload);

    // Create response
    std::vector<uint8_t> createResponse(uint16_t code, const std::string& client_id, const std::vector<uint8_t>& additional_data);

    // Generate client ID
    std::string generateClientId();

    // Helper functions for specific message types
    std::vector<uint8_t> createRegisterRequest(const std::string& client_id, const std::string& name);
    std::vector<uint8_t> createPublicKeyRequest(const std::string& client_id, const std::vector<uint8_t>& public_key);
    std::vector<uint8_t> createFileRequest(const std::string& client_id, const std::string& filename);
    std::vector<uint8_t> createValidCrcRequest(const std::string& client_id, const std::string& filename);
    std::vector<uint8_t> createInvalidCrcRequest(const std::string& client_id, const std::string& filename);
    std::vector<uint8_t> createFourthAesKeyRequest(const std::string& client_id);

private:
    // You can add private members here if needed
};