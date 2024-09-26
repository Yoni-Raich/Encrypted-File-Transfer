#pragma once
#include <string>
#include <cryptopp/rsa.h>
#include <cryptopp/secblock.h>


class CryptoManager {
public:
    CryptoManager();

    void generateRSAKeys();
    std::string getEncryptedAESKey();
    void generateAESKey();
    std::string encryptAES(const std::string& data);
    std::string decryptAES(const std::string& data);
    std::string encryptRSA(const CryptoPP::SecByteBlock& data);
    std::string decryptRSA(const std::string& encryptedData);
    std::string getCRC(const std::string& data, bool decrypt = false);

    CryptoPP::RSA::PrivateKey getPrivateKey();
    std::string getPublicKey();
    CryptoPP::SecByteBlock getAESKey();
    void setAESKey(const CryptoPP::SecByteBlock& key);

private:
    CryptoPP::RSA::PrivateKey privateKey;
    CryptoPP::RSA::PublicKey publicKey;
    CryptoPP::SecByteBlock aesKey;
};