#include "Client.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <filesystem>

Client::Client(const std::string& server_ip, const std::string& server_port, const std::string& username, const std::string& file_path, FileManager& file_manager)
	: name_(username),
	file_path_(file_path),
	protocol_(),
	network_manager_(server_ip, server_port),
	crypto_manager_(),
	file_manager_(file_manager) {}

Client::~Client()
{
}

void Client::run() {
	try {
		network_manager_.connect();

		if (!init_client_data()) {
			register_to_server();
			std::cout << "You are now registered to the server\n\n" << std::endl;
			perform_initial_key_exchange();
			std::cout << "Initial key exchange completed\n\n" << std::endl;
		}
		else {
			perform_reconnect();
			std::cout << "Reconnect completed\n\n" << std::endl;
		}

		send_file();
		std::cout << "File sent successfully" << std::endl;

		if (!get_approved_message()) {
			throw std::runtime_error("Server responded with an unknown error code");
		}
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		network_manager_.disconnect();
		exit(1);
	}
	network_manager_.disconnect();
}

bool Client::init_client_data() {
	if (file_manager_.readMeInfo()) {
		client_id_ = bytes_to_uuid_string(file_manager_.getClientId());
		crypto_manager_.setPrivateKey(file_manager_.getKey());
		return true;
	}
	return false;
}

void Client::register_to_server() {
	for (size_t attempt = 0; attempt < Protocol::MAX_RETRY_ATTEMPTS; ++attempt)
	{
		std::cout << "Attempting to register to the server with name: " << name_ << std::endl;
		std::vector<uint8_t> request = protocol_.create_register_request(string_to_binary(client_id_, Protocol::CLIENT_ID_SIZE), name_);
		network_manager_.sendRequest(request);

		auto [version, code, payload] = protocol_.parse_response(network_manager_.receiveResponse());

		if (code == Protocol::REGISTER_SUCCESS_CODE && !payload.empty()) {
			client_id_ = bytes_to_uuid_string(payload);
			std::cout << "Registration successful. Client ID: " << client_id_ << std::endl;
			file_manager_.setClientId(payload);
			return;
		}

		std::cout << protocol_.SERVER_RESPOND_ERROR << std::endl;
	}

	handle_server_error(Protocol::REGISTER_FAILURE_CODE, "Registration");
}

void Client::perform_initial_key_exchange() {
	crypto_manager_.generateRSAKeys();
	std::vector<uint8_t> public_key = crypto_manager_.getPublicKey();
	std::vector<uint8_t> name_bin = string_to_binary(name_, Protocol::MAX_NAME_LENGTH);
	name_bin.insert(name_bin.end(), public_key.begin(), public_key.end());
	std::cout << "Sending public key to server" << std::endl;
	std::vector<uint8_t> request = protocol_.create_public_key_request(get_client_id_binary(), name_bin);

	file_manager_.setKey(crypto_manager_.getStringPrivateKey());
	file_manager_.writeMeInfo();

	receive_and_process_aes_key(request);
}

void Client::perform_reconnect() {
	std::vector<uint8_t> name_bin = string_to_binary(name_, Protocol::MAX_NAME_LENGTH);
	std::vector<uint8_t> request = protocol_.create_reconnect_request(get_client_id_binary(), name_bin);
	std::cout << "Attempting to reconnect to the server" << std::endl;
	receive_and_process_aes_key(request);
}

void Client::receive_and_process_aes_key(const std::vector<uint8_t>& request) {
	for (size_t attempt = 0; attempt < Protocol::MAX_RETRY_ATTEMPTS; ++attempt) {
		network_manager_.sendRequest(request);

		auto [version, code, payload] = protocol_.parse_response(network_manager_.receiveResponse());
		std::vector<uint8_t> client_id_bytes(payload.begin(), payload.begin() + Protocol::CLIENT_ID_SIZE);
		std::string received_client_id = bytes_to_uuid_string(client_id_bytes);
		if ((code == Protocol::KEY_EXCHANGE_SUCCESS_CODE || code == Protocol::RECONNECT_SUCCESS_CODE) && received_client_id == client_id_) {
			std::vector<uint8_t> encrypted_aes_key(payload.begin() + Protocol::CLIENT_ID_SIZE, payload.end());
			std::string aes_key = crypto_manager_.decryptRSA(encrypted_aes_key);
			std::cout << "Received AES key\n " << aes_key << std::endl;
			crypto_manager_.setAESKey(aes_key);
			return;
		}
		std::cout << protocol_.SERVER_RESPOND_ERROR << std::endl;
	}

	handle_server_error(Protocol::KEY_EXCHANGE_FAILURE_CODE, "Key exchange");
}

void Client::send_file() {
	std::ifstream file(file_path_, std::ios::binary);
	if (!file) {
		throw std::runtime_error("Failed to open file: " + file_path_);
	}

	std::string filename = std::filesystem::path(file_path_).filename().string();
	std::vector<uint8_t> file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();

	std::vector<uint8_t> encrypted_content = crypto_manager_.encryptAES(file_content);
	uint32_t total_packets = encrypted_content.size() / Protocol::MAX_FILE_PACKET_SIZE + 1;
	std::vector<uint8_t> client_id_binary = get_client_id_binary();
	std::cout << "Sending file: " << filename << " with " << total_packets << " packets" << std::endl;
	std::cout << "File size: " << encrypted_content.size() << " bytes" << std::endl;
	for (size_t i = 1; i <= total_packets; i++) {
		std::vector<uint8_t> request = protocol_.create_file_request(client_id_binary, filename, file_content.size(), encrypted_content, i);

		i == 1 ? network_manager_.sendRequest(request) : network_manager_.sendFilePacket(request, Protocol::FILE_PACKET_SIZE);
	}

	auto [version, code, payload] = protocol_.parse_response(network_manager_.receiveResponse());

	std::cout << "Server responded with code: " << code << std::endl;
	if (code == Protocol::FILE_RECEIVED_SUCCESS_CODE) {
		if (!handle_crc_response(payload)) {
			std::vector<uint8_t> request = protocol_.create_crc_request(Protocol::CRC_RETRY_CODE, client_id_binary, filename);
			network_manager_.sendRequest(request);
			if (get_approved_message()) {
				throw std::runtime_error(protocol_.FATAL_CRC_ERROR);
			}
		}
		return;
	}

	handle_server_error(code, "File transfer");
}

bool Client::handle_crc_response(const std::vector<uint8_t>& payload) {
	std::string filename(payload.begin() + Protocol::CLIENT_ID_SIZE + 4, payload.begin() + Protocol::MAX_NAME_LENGTH);
	uint32_t server_crc = (payload[payload.size() - 4] << 24) | (payload[payload.size() - 3] << 16)
		| (payload[payload.size() - 2] << 8) | payload[payload.size() - 1];

	uint32_t current_crc = readfile(file_path_);
	std::cout << "Try to compare crc: " << current_crc << " with server crc: " << server_crc << std::endl;
	if (current_crc == server_crc) {
		std::vector<uint8_t> request = protocol_.create_crc_request(Protocol::CRC_OK_CODE, get_client_id_binary(), filename);
		network_manager_.sendRequest(request);
		return true;
	}
	return false;
}

std::vector<uint8_t> Client::get_client_id_binary() const {
	return uuid_string_to_binary(client_id_);
}

bool Client::get_approved_message() {
	auto [version, code, payload] = protocol_.parse_response(network_manager_.receiveResponse());

	if (code == Protocol::GENERAL_SUCCESS_CODE) {
		std::cout << "Server sent approval with code: " << code << ". Will disconnect." << std::endl;
		return true;
	}
	return false;
}

void Client::handle_server_error(uint16_t code, const std::string& context) {
	std::string error = protocol_.FATAL_REGISTER_ERROR;
	error += " Server responded with " + std::string(code == Protocol::REGISTER_FAILURE_CODE ? "an error" : "an unknown error") + " code: " + std::to_string(code);
	error += " during " + context;
	throw std::runtime_error(error);
}

std::string Client::bytes_to_uuid_string(const std::vector<uint8_t>& bytes) {
	std::stringstream ss;
	ss << std::hex << std::setfill('0');
	for (size_t i = 0; i < bytes.size(); ++i) {
		ss << std::setw(2) << static_cast<int>(bytes[i]);
		if (i == 3 || i == 5 || i == 7 || i == 9) {
			ss << '-';
		}
	}
	return ss.str();
}

std::vector<uint8_t> Client::string_to_binary(const std::string& input, size_t desired_size) {
	std::vector<uint8_t> result(desired_size, 0);
	std::copy(input.begin(), input.end(), result.begin());
	return result;
}

std::vector<uint8_t> Client::uuid_string_to_binary(const std::string& uuid) {
	std::string uuid_no_hyphens;
	for (char c : uuid) {
		if (c != '-') {
			uuid_no_hyphens += c;
		}
	}

	std::vector<uint8_t> binary;
	for (size_t i = 0; i < uuid_no_hyphens.length(); i += 2) {
		std::string byte = uuid_no_hyphens.substr(i, 2);
		binary.push_back(static_cast<uint8_t>(std::stoi(byte, nullptr, 16)));
	}

	return binary;
}