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
#include "CryptoManager.h"

using namespace CryptoPP;



CryptoManager::CryptoManager() : aesKey(AES::DEFAULT_KEYLENGTH) {}

void CryptoManager::generateRSAKeys()
{
        AutoSeededRandomPool rng;
        InvertibleRSAFunction params;
        params.GenerateRandomWithKeySize(rng, 1024);

        privateKey = RSA::PrivateKey(params);
        publicKey = RSA::PublicKey(params);
}

    std::string CryptoManager::getEncryptedAESKey()
    {
        return encryptRSA(aesKey);
    }

    //TODO need to remove this function
    void CryptoManager::generateAESKey()
    {
        AutoSeededRandomPool rng;
        rng.GenerateBlock(aesKey, aesKey.size());
    }

    std::string CryptoManager::encryptAES(const std::string& data) {
        AutoSeededRandomPool rng;
        byte iv[AES::BLOCKSIZE];
        rng.GenerateBlock(iv, sizeof(iv));

        std::string ciphertext;
        CBC_Mode<AES>::Encryption encryption;
        encryption.SetKeyWithIV(aesKey, aesKey.size(), iv);

        StringSource ss(data, true,
            new StreamTransformationFilter(encryption,
                new StringSink(ciphertext)
            )
        );

        return std::string(reinterpret_cast<char*>(iv), AES::BLOCKSIZE) + ciphertext;
    }

    std::string CryptoManager::decryptAES(const std::string& data) {
        byte iv[AES::BLOCKSIZE];
        memcpy(iv, data.data(), AES::BLOCKSIZE);

        std::string ciphertext = data.substr(AES::BLOCKSIZE);
        std::string plaintext;

        CBC_Mode<AES>::Decryption decryption;
        decryption.SetKeyWithIV(aesKey, aesKey.size(), iv);

        StringSource ss(ciphertext, true,
            new StreamTransformationFilter(decryption,
                new StringSink(plaintext)
            )
        );

        return plaintext;
    }

    std::string CryptoManager::encryptRSA(const SecByteBlock& data) {
        AutoSeededRandomPool rng;
        std::string ciphertext;

        RSAES_OAEP_SHA_Encryptor encryptor(publicKey);
        StringSource ss(data.data(), data.size(), true,
            new PK_EncryptorFilter(rng, encryptor,
                new StringSink(ciphertext)
            )
        );

        return ciphertext;
    }

    std::string CryptoManager::decryptRSA(const std::string& encryptedData) {
        AutoSeededRandomPool rng;
        std::string decryptedData;

        RSAES_OAEP_SHA_Decryptor decryptor(privateKey);
        StringSource ss(encryptedData, true,
            new PK_DecryptorFilter(rng, decryptor,
                new StringSink(decryptedData)
            )
        );

        return decryptedData;
    }

    /*std::string CryptoManager::getCRC(const std::string& data, bool decrypt = false) {
        std::string processedData = data;
        if (decrypt) {
            processedData = decryptAES(data);
        }

        CRC32 crc;
        std::string hash;
        StringSource ss(processedData, true,
            new HashFilter(crc,
                new HexEncoder(
                    new StringSink(hash)
                )
            )
        );

        return hash;
    }*/
    // get the keys
    RSA::PrivateKey CryptoManager::getPrivateKey() {
		return privateKey;
	}

    std::string CryptoManager::getPublicKey() {
        ByteQueue queue;
        publicKey.DEREncodePublicKey(queue);
        std::string publicKeyStr;
        CryptoPP::Base64Encoder encoder(new CryptoPP::StringSink(publicKeyStr), false);
        queue.CopyTo(encoder);
        encoder.MessageEnd();
        return publicKeyStr;
    }

    SecByteBlock CryptoManager::getAESKey() {
		return aesKey;
	}

    /*void setAESKey(const SecByteBlock& key) {
        CryptoManager::aesKey = key;
	}*/



//int main() {
//    CryptoManager cryptoManager;
//    cryptoManager.generateRSAKeys();
//    cryptoManager.generateAESKey();
//    std::string encAESKey = cryptoManager.getEncryptedAESKey();
//    // get the keys as RSA::PrivateKey and RSA::PublicKey format and convert them to string and print them
//    RSA::PrivateKey privateKey = cryptoManager.getPrivateKey();
//    RSA::PublicKey publicKey = cryptoManager.getPublicKey();
//    std::string strPrivateKey, strPublicKey;
//    privateKey.Save(StringSink(strPrivateKey).Ref());
//    publicKey.Save(StringSink(strPublicKey).Ref());
//    std::cout << "Private Key:\n " << strPrivateKey << std::endl;
//    
//    
//    
//
//    return 0;
//}