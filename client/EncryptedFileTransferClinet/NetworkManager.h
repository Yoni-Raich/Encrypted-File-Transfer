#pragma once

#include <string>
#include <vector>
#include <boost/asio.hpp>
#define MAX_LENGTH 1024

class NetworkManager {
public:
    NetworkManager(const std::string& server_ip, const std::string& server_port);
    ~NetworkManager();

    void connect();
    void disconnect();
    void sendData(const std::string& data);
    void sendData(const std::vector<uint8_t>& data);
    std::string receive_data_string();
    std::vector<uint8_t> receiveData();

private:
    boost::asio::io_context m_io_context;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::ip::tcp::resolver m_resolver;
    std::string m_server_ip;
    std::string m_server_port;

    template<typename T>
    void send_data_impl(const T& data);

    template<typename T>
    //T receive_data_impl();
};