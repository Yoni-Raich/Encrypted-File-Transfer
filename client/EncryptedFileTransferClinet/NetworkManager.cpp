#include "NetworkManager.h"
#include <cryptopp/rsa.h>
#include <boost/endian/conversion.hpp>

using boost::asio::ip::tcp;
using namespace CryptoPP;

NetworkManager::NetworkManager(const std::string& server_ip, const std::string& server_port)
    : m_socket(m_io_context), m_resolver(m_io_context), m_server_ip(server_ip), m_server_port(server_port) {}

NetworkManager::~NetworkManager() {
    disconnect();
}

void NetworkManager::connect() {
    boost::asio::connect(m_socket, m_resolver.resolve(m_server_ip, m_server_port));
}

void NetworkManager::disconnect() {
    if (m_socket.is_open()) {
        m_socket.close();
    }
}

void NetworkManager::sendData(const std::string& data) {
    send_data_impl(data);
}

void NetworkManager::sendData(const std::vector<uint8_t>& data) {
    send_data_impl(data);
}
//
//std::string NetworkManager::receive_data_string() {
//    return receive_data_impl<std::string>();
//}
//
//std::vector<uint8_t> NetworkManager::receiveData() {
//    return receive_data_impl<std::vector<uint8_t>>();
//}
//
//template<typename T>
//void NetworkManager::send_data_impl(const T& data) {
//    uint32_t message_length = boost::endian::native_to_big(static_cast<uint32_t>(data.size()));
//    boost::asio::write(m_socket, boost::asio::buffer(&message_length, sizeof(message_length)));
//    boost::asio::write(m_socket, boost::asio::buffer(data));
//}

//template<typename T>
//T NetworkManager::receive_data_impl() {
//    uint32_t message_length;
//    boost::asio::read(m_socket, boost::asio::buffer(&message_length, sizeof(message_length)));
//    message_length = boost::endian::big_to_native(message_length);
//
//    T received_data(message_length);
//    boost::asio::read(m_socket, boost::asio::buffer(received_data));
//
//    return received_data;
//}

// Explicit template instantiations
//template void NetworkManager::send_data_impl<std::string>(const std::string&);
//template void NetworkManager::send_data_impl<std::vector<uint8_t>>(const std::vector<uint8_t>&);
//template std::string NetworkManager::receive_data_impl<std::string>();
//template std::vector<uint8_t> NetworkManager::receive_data_impl<std::vector<uint8_t>>();