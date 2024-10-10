#pragma once
#include <string>
#include <cryptopp/rsa.h>
#include <cryptopp/secblock.h>
#include <vector>



class CryptoManager {
public:
    static const size_t AES_KEY_SIZE = 32; // 256 bits
    
    CryptoManager();

    void generateRSAKeys();
    std::string getEncryptedAESKey();
    void generateAESKey();
    std::vector<uint8_t> encryptAES(const std::vector<uint8_t>& data);
    std::string decryptAES(const std::vector<uint8_t>& data);
    std::string encryptRSA(const std::string plainText);
    std::string decryptRSA(const std::vector<uint8_t>& encryptedData);
    std::string getCRC(const std::string& data, bool decrypt = false);
    std::vector<uint8_t> getPublicKey();

    CryptoPP::RSA::PrivateKey getPrivateKey();
    CryptoPP::SecByteBlock getAESKey();
    void setAESKey(const std::string& keyString);
	void setRsaPrivateKey(const std::string& keyString);

private:
    CryptoPP::RSA::PrivateKey privateKey;
    CryptoPP::RSA::PublicKey publicKey;
    CryptoPP::SecByteBlock aesKey;
};