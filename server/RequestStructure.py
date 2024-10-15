import struct
import uuid

class RequestStructure:
    NAME_MAX_LENGTH = 255
    PUBLIC_KEY_LENGTH = 160

    def __init__(self, client_id, version, code, payload_size, payload):
        uuid_obj = uuid.UUID(bytes=client_id)
        self.client_id = str(uuid_obj)
        self.version = version
        self.code = code
        self.payload_size = payload_size
        self.payload = payload

    def get_name(self):
        return self.payload[:self.NAME_MAX_LENGTH].decode('ascii').strip('\x00')

    def get_public_key(self):
        return self.payload[self.NAME_MAX_LENGTH:self.NAME_MAX_LENGTH + self.PUBLIC_KEY_LENGTH].rstrip(b'\x00')

    def get_file_structure(self, packet=None, packet_number=None):
        if packet:
            self.payload = packet
        
        content_size, orig_file_size, packet_number, total_packet = struct.unpack('!IIHH', self.payload[:12])
        filename = self.payload[12:12 + self.NAME_MAX_LENGTH].decode('ascii').strip('\x00')
        encrypted_file_content = self.payload[12 + self.NAME_MAX_LENGTH:]
        
        packet_size = len(encrypted_file_content)
        total_size = packet_size * total_packet
        padding_size = total_size - content_size    
        print(packet_size)   
        print(total_size) 
        print(padding_size)
        
        # Remove padding if it's the last packet
        if packet_number == total_packet:
            encrypted_file_content = encrypted_file_content[:-padding_size]
        
        file_structure = {
            'content_size': content_size,
            'file_size': orig_file_size,
            'packet_number': packet_number,
            'total_packet': total_packet,
            'filename': filename,
            'encrypted_file_content': encrypted_file_content
        }
        
        return file_structure
        
            
            

