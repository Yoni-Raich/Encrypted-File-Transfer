#include "FileManager.h"
#include <fstream>
#include <sstream>
#include <regex>

// Define the constant file paths
const std::string FileManager::TRANSFER_INFO_PATH = "transfer.info";
const std::string FileManager::ME_INFO_PATH = "me.info";

FileManager::FileManager() : m_port("") {}

FileManager::~FileManager() {}

bool FileManager::readTransferInfo() {
    std::ifstream file(TRANSFER_INFO_PATH);
    if (!file.is_open()) {
        return false;
    }

    std::string line;
    if (std::getline(file, line)) {
        std::size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            return false;
        }
        m_ipAddress = line.substr(0, colonPos);
        m_port = line.substr(colonPos + 1);
    } else {
        return false;
    }

    if (std::getline(file, m_username) && std::getline(file, m_filename)) {
        return validateIpAddress(m_ipAddress) && validatePort(m_port) &&
               validateUsername(m_username) && validateFilename(m_filename);
    }

    return false;
}

bool FileManager::readMeInfo() {
    std::ifstream file(ME_INFO_PATH);
    if (!file.is_open()) {
        return false;
    }

    std::string name;
    if (std::getline(file, name) && std::getline(file, m_clientId) && std::getline(file, m_key)) {
        if (!m_username.empty() && name != m_username) {
            throw std::runtime_error("Name in me.info does not match username in transfer.info");
        }
        if (m_username.empty()) {
            m_username = name;
        }
        return validateUsername(m_username) && validateClientId(m_clientId) && validateKey(m_key);
    }

    return false;
}

bool FileManager::writeMeInfo() {
    std::ofstream file(ME_INFO_PATH);
    if (!file.is_open()) {
        return false;
    }

    file << m_username << std::endl;
    file << m_clientId << std::endl;
    file << m_key << std::endl;

    return file.good();
}

std::string FileManager::getIpAddress() const {
    return m_ipAddress;
}

std::string FileManager::getPort() const {
    return m_port;
}

std::string FileManager::getUsername() const {
    return m_username;
}

std::string FileManager::getFilename() const {
    return m_filename;
}

std::string FileManager::getClientId() const {
    return m_clientId;
}

std::string FileManager::getKey() const {
    return m_key;
}

void FileManager::setClientId(const std::string& clientId) {
    if (validateClientId(clientId)) {
        m_clientId = clientId;
    } else {
        throw std::invalid_argument("Invalid client ID");
    }
}

void FileManager::setKey(const std::string& key) {
    if (validateKey(key)) {
        m_key = key;
    } else {
        throw std::invalid_argument("Invalid key");
    }
}

bool FileManager::validateIpAddress(const std::string& ip) {
    std::regex ipPattern("^(?:[0-9]{1,3}\\.){3}[0-9]{1,3}$");
    if (!std::regex_match(ip, ipPattern)) {
        return false;
    }
    // Additional check for valid IP ranges
    std::istringstream iss(ip);
    int a, b, c, d;
    char dot;
    iss >> a >> dot >> b >> dot >> c >> dot >> d;
    return a >= 0 && a <= 255 && b >= 0 && b <= 255 && 
           c >= 0 && c <= 255 && d >= 0 && d <= 255;
}

bool FileManager::validatePort(const std::string& port) {
    try {
        int portNum = std::stoi(port);
        return portNum > 1024 && portNum <= 65535;
    } catch (const std::exception&) {
        return false;
    }
}

bool FileManager::validateUsername(const std::string& username) {
    return !username.empty() && username.length() <= 100;
}

bool FileManager::validateFilename(const std::string& filename) {
    return !filename.empty() && filename.length() <= 255;
}

bool FileManager::validateClientId(const std::string& clientId) {
    // Client ID should be a 32-character hexadecimal string
    std::regex clientIdPattern("^[0-9a-fA-F]{32}$");
    return std::regex_match(clientId, clientIdPattern);
}

bool FileManager::validateKey(const std::string& key) {
    // Key should be a Base64 encoded string of 43 characters ending with '='
    std::regex keyPattern("^[A-Za-z0-9+/]{43}=$");
    return std::regex_match(key, keyPattern);
}
