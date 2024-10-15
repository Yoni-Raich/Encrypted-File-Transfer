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

void NetworkManager::sendRequest(const std::vector<uint8_t>& request) {
    if (request.size() < REQUEST_HEADER_SIZE) {
        throw std::runtime_error("Invalid request size");
    }

    // Send the fixed-size header
    boost::asio::write(m_socket, boost::asio::buffer(request.data(), REQUEST_HEADER_SIZE));

    // Extract payload size from the header
    uint32_t payload_size = (request[19] << 24) | (request[20] << 16) | (request[21] << 8) | request[22];

    // Send the payload
    size_t actual_payload_size = request.size() - REQUEST_HEADER_SIZE;
    boost::asio::write(m_socket, boost::asio::buffer(request.data() + REQUEST_HEADER_SIZE, actual_payload_size));

    // If the payload is smaller than the specified size, pad with zeros
    if (actual_payload_size < payload_size) {
        std::vector<uint8_t> padding(payload_size - actual_payload_size, 0);
        boost::asio::write(m_socket, boost::asio::buffer(padding));
    }
}

void NetworkManager::sendFilePacket(const std::vector<uint8_t>& request, const size_t packetSize)
{
    if (request.size() < packetSize)
    {
		throw std::runtime_error("Invalid file packet size");
	}

    boost::asio::write(m_socket, boost::asio::buffer(request.data(), packetSize));
}

std::vector<uint8_t> NetworkManager::receiveResponse() {
    std::vector<uint8_t> header(RESPONSE_HEADER_SIZE);
    boost::asio::read(m_socket, boost::asio::buffer(header));

    uint32_t payload_size = (header[3] << 24) | (header[4] << 16) | (header[5] << 8) | header[6];
    
    std::vector<uint8_t> response = header;
    response.resize(RESPONSE_HEADER_SIZE + payload_size);
    
    boost::asio::read(m_socket, boost::asio::buffer(response.data() + RESPONSE_HEADER_SIZE, payload_size));

    return response;
}