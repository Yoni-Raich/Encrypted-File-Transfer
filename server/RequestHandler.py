import struct
from Crypto.PublicKey import RSA
from Protocol import Protocol

class RequestHandler:
    def __init__(self):
        self.protocol = Protocol()

    def handle_register(self, client_id, payload):
        name = payload.decode('utf-8').strip('\x00')
        if self.protocol.client_manager.add_client(client_id, name):
            return self.protocol.create_response(1600, client_id)
        else:
            return self.protocol.create_response(1601)

    def handle_public_key(self, client_id, payload):
        name = payload[:255].decode('utf-8').strip('\x00') #TODO: Check why name is not used
        public_key = RSA.import_key(payload[255:]) #TODO: why you need to import the key
        self.protocol.client_manager.update_client(client_id, public_key=public_key)
        
        self.protocol.crypto_manager.public_key = public_key
        self.protocol.crypto_manager.generate_aes_key()
        enc_aes_key = self.protocol.crypto_manager.get_encrypted_aes_key()
        
        self.protocol.client_manager.update_client(client_id, aes_key=self.protocol.crypto_manager.aes_key)
        return self.protocol.create_response(1602, client_id, enc_aes_key)

    def handle_reconnect(self, client_id, payload):
        name = payload.decode('utf-8').strip('\x00')
        client = self.protocol.client_manager.get_client(client_id)
        if client and client['name'] == name:
            aes_key = client['aes_key']
            public_key = client['public_key']
            self.protocol.crypto_manager.public_key = public_key
            self.protocol.crypto_manager.aes_key = aes_key
            enc_aes_key = self.protocol.crypto_manager.get_encrypted_aes_key()
            return self.protocol.create_response(1605, client_id, enc_aes_key)
        else:
            return self.protocol.create_response(1606, client_id)

    def handle_file_transfer(self, client_id, payload):
        content_size, file_size, chunk_number = struct.unpack('!III', payload[:12])
        filename = payload[12:267].decode('utf-8').strip('\x00')
        encrypted_file_content = payload[267:]
        
        client = self.protocol.client_manager.get_client(client_id)
        if client:
            self.protocol.crypto_manager.aes_key = client['aes_key']
            file_content = self.protocol.crypto_manager.decrypt_aes(encrypted_file_content)
            
            # Calculate CRC
            crc = self.protocol.crypto_manager.get_CRC(file_content)

            return self.protocol.create_response(1603, client_id, filename.encode('utf-8'), crc)
        else:
            return self.protocol.create_response(1607, client_id)  # General error

    def handle_crc_response(self, client_id, code, payload):
        filename = payload.decode('utf-8').strip('\x00')
        response = self.protocol.create_response(1604, client_id)
        
        if code == 900:
            print(f"File {filename} transferred successfully")
        elif code == 901:
            print(f"CRC failed for file {filename}, client will retry")
        elif code == 902:
            print(f"CRC failed for file {filename}, client gave up")
        
        return response