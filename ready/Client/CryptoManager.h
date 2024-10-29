#pragma once
#include <string>
#include <cryptopp/rsa.h>
#include <cryptopp/secblock.h>
#include <vector>
#include <iostream>
#include <string>
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/crc.h>
#include <cryptopp/base64.h>
#include <cryptopp/files.h>
#include <cryptopp/cryptlib.h>



class CryptoManager {
public:
    static const size_t AES_KEY_SIZE = 32; // 256 bits
    
    CryptoManager();

    void generateRSAKeys();
    std::vector<uint8_t> encryptAES(const std::vector<uint8_t>& data);
    std::string decryptAES(const std::vector<uint8_t>& data);
    std::string encryptRSA(const std::string plainText);
    std::string decryptRSA(const std::vector<uint8_t>& encryptedData);
    std::vector<uint8_t> getPublicKey();
    std::string getStringPrivateKey();
    CryptoPP::RSA::PrivateKey getPrivateKey();
    CryptoPP::SecByteBlock getAESKey();
    void setAESKey(const std::string& keyString);
	void setPrivateKey(const std::string& keyString);
	

private:
    CryptoPP::RSA::PrivateKey privateKey;
    CryptoPP::RSA::PublicKey publicKey;
    CryptoPP::SecByteBlock aesKey;
};