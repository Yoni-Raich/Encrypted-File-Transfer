#pragma once

#include "cksum_new.h"
#include "Protocol.h"
#include "NetworkManager.h"
#include "CryptoManager.h"
#include "FileManager.h"
#include <string>
#include <vector>

class Client {
public:
	Client(const std::string& server_ip, const std::string& server_port, const std::string& username, const std::string& file_path, FileManager& file_manager);
	~Client();

	// Main client operation
	void run();

	// Initialize client data from file
	bool init_client_data();

private:
	static const int MAX_RETRY_ATTEMPTS = 3;
	static const size_t UUID_SIZE = 16;
	static const size_t MAX_NAME_LENGTH = 255;

	std::string client_id_;
	std::string name_;
	std::string file_path_;
	Protocol protocol_;
	NetworkManager network_manager_;
	CryptoManager crypto_manager_;
	FileManager& file_manager_;

	// Registration and key exchange
	void register_to_server();
	void perform_initial_key_exchange();
	void perform_reconnect();
	void receive_and_process_aes_key(const std::vector<uint8_t>& request);

	// File operations
	void send_file();
	bool handle_crc_response(const std::vector<uint8_t>& payload);

	// Utility functions
	std::vector<uint8_t> get_client_id_binary() const;
	bool get_approved_message();
	void handle_server_error(uint16_t code, const std::string& context);

	// Conversion functions
	static std::string bytes_to_uuid_string(const std::vector<uint8_t>& bytes);
	static std::vector<uint8_t> string_to_binary(const std::string& input, size_t desired_size);
	static std::vector<uint8_t> uuid_string_to_binary(const std::string& uuid);
};
