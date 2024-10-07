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
#include <cryptopp/files.h>
#include <cryptopp/cryptlib.h>

using namespace CryptoPP;

// Constructor
CryptoManager::CryptoManager() : aesKey(AES_KEY_SIZE) {} // Changed to 32 bytes

void CryptoManager::generateRSAKeys()
{
        AutoSeededRandomPool rng;
        InvertibleRSAFunction params;
        params.GenerateRandomWithKeySize(rng, 1024);

        privateKey = RSA::PrivateKey(params);
        publicKey = RSA::PublicKey(params);
}

    //std::string CryptoManager::getEncryptedAESKey()
    //{
    //    return encryptRSA(aesKey);
    //}

    //TODO need to remove this function
    void CryptoManager::generateAESKey()
    {
        AutoSeededRandomPool rng;
        aesKey.resize(AES_KEY_SIZE); // Ensure the key is 32 bytes
        rng.GenerateBlock(aesKey, aesKey.size());
    }

    std::vector<uint8_t> CryptoManager::encryptAES(const std::vector<uint8_t>& data) {
        // Generate IV
        AutoSeededRandomPool prng;
        byte iv[AES::BLOCKSIZE];
        prng.GenerateBlock(iv, AES::BLOCKSIZE);

        // Set up encryption objects
        CBC_Mode<AES>::Encryption encryptor;
        encryptor.SetKeyWithIV(aesKey, AES_KEY_SIZE, iv);

        // Encrypt
        std::string ciphertext;
        StringSource(
            reinterpret_cast<const byte*>(data.data()), data.size(), true,
            new StreamTransformationFilter(encryptor,
                new StringSink(ciphertext)
            )
        );

        // Prepare result: IV + Ciphertext
        std::vector<uint8_t> result(iv, iv + AES::BLOCKSIZE);
        result.insert(result.end(), ciphertext.begin(), ciphertext.end());

        return result;
    }

    std::string CryptoManager::decryptAES(const std::vector<uint8_t>& data) {
        if (data.size() < AES::BLOCKSIZE) {
            throw std::invalid_argument("Input data is too short");
        }

        // Extract IV
        byte iv[AES::BLOCKSIZE];
        std::copy(data.begin(), data.begin() + AES::BLOCKSIZE, iv);

        // Set up decryption objects
        CBC_Mode<AES>::Decryption decryptor;
        decryptor.SetKeyWithIV(aesKey, AES_KEY_SIZE, iv);

        // Decrypt
        std::string plaintext;
        StringSource(
            reinterpret_cast<const byte*>(data.data()) + AES::BLOCKSIZE, 
            data.size() - AES::BLOCKSIZE, 
            true,
            new StreamTransformationFilter(decryptor,
                new StringSink(plaintext)
            )
        );

        // Convert result to vector<uint8_t>
        return plaintext;
    }

    std::string CryptoManager::encryptRSA(const std::string plainText)
    {
        SecByteBlock data(reinterpret_cast<const byte*>(plainText.data()), plainText.size());
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

    std::string CryptoManager::decryptRSA(const std::vector<uint8_t>& encryptedData) {
        AutoSeededRandomPool rng;
        std::string decryptedData;

        RSAES_OAEP_SHA_Decryptor decryptor(privateKey);
        StringSource ss(reinterpret_cast<const byte*>(encryptedData.data()), encryptedData.size(), true,
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

    std::vector<uint8_t> CryptoManager::getPublicKey() {
        ByteQueue queue;
        publicKey.DEREncodePublicKey(queue);

        std::vector<uint8_t> publicKeyVec(queue.CurrentSize());
        queue.Get(reinterpret_cast<byte*>(&publicKeyVec[0]), publicKeyVec.size());

        return publicKeyVec;
    }

    SecByteBlock CryptoManager::getAESKey() {
		return aesKey;
	}

    void CryptoManager::setAESKey(const std::string& keyString) 
    {
        SecByteBlock data(reinterpret_cast<const byte*>(keyString.data()), keyString.size());
        if (keyString.length() != AES_KEY_SIZE) {
            throw std::invalid_argument("Invalid AES key length");
        }

        aesKey = data;
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