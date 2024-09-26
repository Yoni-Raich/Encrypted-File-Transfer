#include "NetworkManager.h"
#include <cryptopp/rsa.h>
using boost::asio::ip::tcp;
using namespace CryptoPP;

NetworkManager::NetworkManager(const std::string server_ip, std::string server_port)
    : m_socket(m_io_context), m_resolver(m_io_context), m_server_ip(server_ip), m_server_port(server_port) {}

NetworkManager::~NetworkManager() {
    disconnect();
}

void NetworkManager::connect() 
{
    boost::asio::connect(m_socket, m_resolver.resolve(m_server_ip, m_server_port));
}

void NetworkManager::disconnect() 
{
    if (m_socket.is_open()) {
        m_socket.close();
    }
}

void NetworkManager::sendData(const std::string data)
{
    boost::asio::write(m_socket, boost::asio::buffer(data, MAX_LENGTH));
}


std::vector<uint8_t> NetworkManager::receiveData() {
    std::vector<uint8_t> received_data(1024);
    size_t length = m_socket.read_some(boost::asio::buffer(received_data));
    received_data.resize(length);
    return received_data;
}