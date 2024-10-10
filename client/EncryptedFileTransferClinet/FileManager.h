#pragma once

#include <string>
#include <stdexcept>

class FileManager {
public:
    FileManager();
    ~FileManager();

    // Methods for transfer.info file
    bool readTransferInfo();
    std::string getIpAddress() const;
    std::string getPort() const;
    std::string getUsername() const;
    std::string getFilename() const;

    // Methods for me.info file
    bool readMeInfo();
    bool writeMeInfo();
    std::string getClientId() const;
    std::string getKey() const;

    // Setters for me.info
    void setClientId(const std::string& clientId);
    void setKey(const std::string& key);

private:
    // Constants for file paths
    static const std::string TRANSFER_INFO_PATH;
    static const std::string ME_INFO_PATH;

    // Data members for transfer.info and me.info
    std::string m_ipAddress;
    std::string m_port;
    std::string m_username;  // Used for both transfer.info and me.info
    std::string m_filename;
    std::string m_clientId;
    std::string m_key;

    // Helper methods
    bool validateIpAddress(const std::string& ip);
    bool validatePort(const std::string& port);
    bool validateUsername(const std::string& username);
    bool validateFilename(const std::string& filename);
    bool validateClientId(const std::string& clientId);
    bool validateKey(const std::string& key);
};
