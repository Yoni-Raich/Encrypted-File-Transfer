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

void NetworkManager::sendData(const std::vector<uint8_t>& data) {
	uint32_t message_length = htonl(data.size());
	//boost::asio::write(m_socket, boost::asio::buffer(&message_length, sizeof(message_length)));        
	boost::asio::write(m_socket, boost::asio::buffer(data));
}


std::string NetworkManager::receiveData() {

    uint32_t message_length;
    boost::asio::read(m_socket, boost::asio::buffer(&message_length, sizeof(message_length)));
    message_length = ntohl(message_length);

    boost::asio::streambuf buffer;
    boost::asio::read(m_socket, buffer, boost::asio::transfer_exactly(message_length));
    std::istream is(&buffer);
    std::string received_data((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

    return received_data;
}

std::vector<uint8_t> NetworkManager::receiveDataBytes() {
	uint32_t message_length;
	boost::asio::read(m_socket, boost::asio::buffer(&message_length, sizeof(message_length)));
	message_length = ntohl(message_length);

	std::vector<uint8_t> received_data(message_length);
	boost::asio::read(m_socket, boost::asio::buffer(received_data));

	return received_data;
}