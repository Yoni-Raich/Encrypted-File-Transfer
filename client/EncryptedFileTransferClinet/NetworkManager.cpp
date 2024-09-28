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


std::string NetworkManager::receiveData() 
{
    char received_data[MAX_LENGTH];
    m_socket.read_some(boost::asio::buffer(received_data));
    return received_data;
}