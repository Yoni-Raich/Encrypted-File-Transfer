import struct
from Crypto.PublicKey import RSA
from Protocol import Protocol
from CryptoManager import CryptoManager
from ClientManager import ClientManager
from RequestStructure import RequestStructure


class RequestHandler:
    def __init__(self):
        self.protocol = Protocol()
        self.client_manager = ClientManager()
        self.crypto_manager = None
        self.receive_request = None

    def new_request(self, receive_request: RequestStructure):
        self.receive_request = receive_request

    def create_response(self):
        code = self.receive_request.code
        client_id = self.receive_request.client_id
        payload = self.receive_request.payload

        if code == 825:  # Register request
            response = self.handle_register(client_id, payload)
        elif code == 826:  # Send public key
            response = self.handle_public_key(client_id, payload)
        elif code == 827:  # Reconnect
            response = self.handle_reconnect(client_id, payload)
        elif code == 828:  # Send file
            response = self.handle_file_transfer(client_id, payload)
        elif code in [900, 901, 902]:  # CRC responses
            response = self.handle_crc_response(client_id, code, payload)
        else:
            response = self.protocol.create_response(1607, client_id)  # General error

        return response

    def handle_register(self, client_id, payload):
        name = self.receive_request.get_name()
        try:
            client_id = self.client_manager.add_client(name)
            return self.protocol.create_response(1600, client_id)
        except Exception as e:
            print(f"Error registering client: {e}")
            return self.protocol.create_response(1601)

    def handle_public_key(self, client_id, payload):
        name = self.receive_request.get_name()
        public_key = self.receive_request.get_public_key()
        self.client_manager.update_client(client_id, public_key=public_key)
        self.crypto_manager = CryptoManager(public_key)
        self.crypto_manager.generate_aes_key()
        enc_aes_key = self.crypto_manager.get_encrypted_aes_key()
        
        self.client_manager.update_client(client_id, aes_key=self.crypto_manager.aes_key)
        return self.protocol.create_response(1602, client_id, enc_aes_key)

    def handle_reconnect(self, client_id, payload):
        name = self.receive_request.get_name()
        client = self.client_manager.get_client(client_id)
        if client and client['name'] == name:
            aes_key = client['aes_key']
            public_key = client['public_key']
            crypto_manager = CryptoManager(public_key)
            crypto_manager.aes_key = aes_key
            enc_aes_key = crypto_manager.get_encrypted_aes_key()
            return self.protocol.create_response(1605, client_id, enc_aes_key)
        else:
            return self.protocol.create_response(1606, client_id)

    def handle_file_transfer(self, client_id, payload):
        file_structure = self.receive_request.get_file_structure()
        num_of_packets = file_structure['total_packet']
        
        client = self.client_manager.get_client(client_id)
        if client:
            crypto_manager = CryptoManager(client['public_key'])
            crypto_manager.aes_key = client['aes_key']
            file_content = crypto_manager.decrypt_aes(file_structure['encrypted_file_content'])
            
            # Calculate CRC
            crc = crypto_manager.get_CRC(file_content)

            return self.protocol.create_response(1603, client_id, file_structure['filename'].encode('utf-8'), crc)
        else:
            return self.protocol.create_response(1607, client_id)  # General error

    def handle_crc_response(self, client_id, code, payload):
        filename = self.receive_request.get_name()
        response = self.protocol.create_response(1604, client_id)
        
        if code == 900:
            print(f"File {filename} transferred successfully")
        elif code == 901:
            print(f"CRC failed for file {filename}, client will retry")
        elif code == 902:
            print(f"CRC failed for file {filename}, client gave up")
        
        return response