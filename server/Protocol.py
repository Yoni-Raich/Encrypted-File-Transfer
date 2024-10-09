import struct
import uuid

from RequestStructure import RequestStructure

class Protocol:
    CLIENT_ID_LENGTH = 16
    VERSION_LENGTH = 1
    CODE_LENGTH = 2
    PAYLOAD_SIZE_LENGTH = 4

    # Response codes
    REGISTER_SUCCESS = 1600
    REGISTER_FAIL = 1601
    AES_KEY_SENT = 1602
    FILE_RECEIVED_WITH_CRC = 1603
    THANK_YOU = 1604
    RECONNECT_APPROVED = 1605
    RECONNECT_DENIED = 1606
    GENERAL_ERROR = 1607

    # Other constants
    VERSION = 3
    FILENAME_LENGTH = 255

    def __init__(self):
        pass

    def parse_request(self, data):
        if len(data) < 24:
            raise ValueError("Data is too short to contain a valid request")

        # Parse the header with big-endian byte order
        header = data[:23]
        client_id, version, code, payload_size = struct.unpack('>16s B H I', header)
        
        # Extract the payload
        payload = data[23:23+payload_size]

        return RequestStructure(client_id, version, code, payload_size, payload)

    def create_response(self, code, client_id=None, *args):
        if client_id and isinstance(client_id, str):
            uuid_obj = uuid.UUID(client_id)
            client_id = uuid_obj.bytes
            
        if code == self.REGISTER_SUCCESS:
            return struct.pack('!BHI16s', self.VERSION, code, self.CLIENT_ID_LENGTH, client_id)
        elif code == self.REGISTER_FAIL:
            return struct.pack('!BHI', self.VERSION, code, 0)
        elif code == self.AES_KEY_SENT:
            enc_aes_key = args[0]
            return struct.pack(f'!BHI16s{len(enc_aes_key)}s', self.VERSION, code, self.CLIENT_ID_LENGTH + len(enc_aes_key), client_id, enc_aes_key)
        elif code == self.FILE_RECEIVED_WITH_CRC:
            content_size, filename, crc = args
            return struct.pack(f'!BHI16sI{self.FILENAME_LENGTH}sI', self.VERSION, code, self.CLIENT_ID_LENGTH + 4 + self.FILENAME_LENGTH + 4, client_id, content_size, filename, crc)
        elif code in [self.THANK_YOU, self.RECONNECT_DENIED]:
            return struct.pack('!BHI16s', self.VERSION, code, self.CLIENT_ID_LENGTH, client_id)
        elif code == self.RECONNECT_APPROVED:
            enc_aes_key = args[0]
            return struct.pack(f'!BHI16s{len(enc_aes_key)}s', self.VERSION, code, self.CLIENT_ID_LENGTH + len(enc_aes_key), client_id, enc_aes_key)
        elif code == self.GENERAL_ERROR:
            return struct.pack('!BHI16s', self.VERSION, code, self.CLIENT_ID_LENGTH, client_id)

if __name__ == "__main__":
    string = b"Hello, World!"
    