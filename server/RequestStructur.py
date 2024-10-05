import struct

class RequestStructure:
    NAME_MAX_LENGTH = 255
    PUBLIC_KEY_LENGTH = 160

    def __init__(self, client_id, version, code, payload_size, payload):
        self.client_id = client_id
        self.version = version
        self.code = code
        self.payload_size = payload_size
        self.payload = payload

    def get_name(self):
        return self.payload[:self.NAME_MAX_LENGTH].decode('utf-8').strip('\x00')

    def get_public_key(self):
        return self.payload[self.NAME_MAX_LENGTH:self.NAME_MAX_LENGTH + self.PUBLIC_KEY_LENGTH].rstrip(b'\x00')

    def get_file_structure(self):
        content_size, file_size, chunk_number = struct.unpack('!III', self.payload[:12])
        filename = self.payload[12:12 + self.NAME_MAX_LENGTH].decode('utf-8').strip('\x00')
        encrypted_file_content = self.payload[12 + self.NAME_MAX_LENGTH:]
        return {
            'content_size': content_size,
            'file_size': file_size,
            'chunk_number': chunk_number,
            'filename': filename,
            'encrypted_file_content': encrypted_file_content
        }

