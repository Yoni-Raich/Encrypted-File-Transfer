/*#pragma once

#include <vector>
#include <memory>
#include <cryptopp/rsa.h>

class CryptoManager {
public:
    CryptoManager(const std::vector<uint8_t>& public_key = std::vector<uint8_t>());

    std::vector<uint8_t> get_encrypted_aes_key(const std::vector<uint8_t>& public_key = std::vector<uint8_t>());
    void generate_rsa_keys();
    std::vector<uint8_t> encrypt_aes(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decrypt_aes(const std::vector<uint8_t>& data);
    std::vector<uint8_t> encrypt_rsa(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decrypt_rsa(const std::vector<uint8_t>& encrypted_data);
    void generate_aes_key();
    std::vector<uint8_t> get_CRC(const std::vector<uint8_t>& data, bool decrypt = false);

private:
    std::string m_public_key;
    std::string m_private_key;
    std::string m_aes_key;
};*/