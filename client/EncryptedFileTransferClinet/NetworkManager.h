#pragma once

#include <string>
#include <vector>
#include <boost/asio.hpp>
#define MAX_LENGTH 1024
class NetworkManager {
public:
    NetworkManager(const std::string server_ip, std::string server_port);
    ~NetworkManager();

    void connect();
    void disconnect();
    void sendData(const std::string data);
    void sendData(const std::vector<uint8_t>& data);
    std::string receiveData();
    std::vector<uint8_t> receiveDataBytes();

    void sendRequest(const std::vector<uint8_t>& request);
    void sebdFilePacket(const std::vector<uint8_t>& request, const size_t packetSize);
    std::vector<uint8_t> receiveResponse();

private:
    boost::asio::io_context m_io_context;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::ip::tcp::resolver m_resolver;
    std::string m_server_ip;
    std::string m_server_port;

    static constexpr size_t REQUEST_HEADER_SIZE = 23;  // 16 (Client ID) + 1 (Version) + 2 (Code) + 4 (Payload size)
    static constexpr size_t RESPONSE_HEADER_SIZE = 7;  // 1 (Version) + 2 (Code) + 4 (Payload size)
    
};