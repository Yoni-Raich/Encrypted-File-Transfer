import struct
from CryptoManager import CryptoManager
from ClientManager import ClientManager

class Protocol:
    CLIENT_ID_LENGTH = 16
    VERSION_LENGTH = 1
    CODE_LENGTH = 2
    PAYLOAD_SIZE_LENGTH = 4

    def __init__(self):
        self.crypto_manager = CryptoManager()
        self.client_manager = ClientManager()

    def parse_request(self, data):
        if len(data) < 24:
            raise ValueError("Data is too short to contain a valid request")

        # פירוק הכותרת (header) עם סדר בתים גדול
        header = data[:23]
        client_id, version, code, payload_size = struct.unpack('>16s B H I', header)

        # המרת client_id למחרוזת רגילה
        client_id = client_id.decode('utf-8').rstrip('\x00')

        # חילוץ ה-payload
        payload = data[23:23+payload_size]

        return client_id, version, code, payload

    def create_response(self, code, client_id=None, *args):
        if code == 1600:  # Register success
            return struct.pack('!BHI16s', 1, code, 16, client_id) #TODO use VERSION  instead just 1
        elif code == 1601:  # Register fail
            return struct.pack('!BHI', 1, code, 0)
        elif code == 1602:  # AES key sent
            enc_aes_key = args[0]
            return struct.pack(f'!BHI16s{len(enc_aes_key)}s', 1, code, 16 + len(enc_aes_key), client_id, enc_aes_key)
        elif code == 1603:  # File received with CRC
            filename, crc = args
            return struct.pack(f'!BHI16sI255sI', 1, code, 16 + 4 + 255 + 4, client_id, len(filename), filename, crc)
        elif code in [1604, 1606]:  # Thank you, Reconnect denied
            return struct.pack('!BHI16s', 1, code, 16, client_id)
        elif code == 1605:  # Reconnect approved
            enc_aes_key = args[0]
            return struct.pack(f'!BHI16s{len(enc_aes_key)}s', 1, code, 16 + len(enc_aes_key), client_id, enc_aes_key)
        elif code == 1607:  # General error
            return struct.pack('!BHI16s', 1, code, 16, client_id)

if __name__ == "__main__":
    protocol = Protocol()
    print(protocol.create_response(1600, "sdfdsf454523fd".ljust(16).encode('utf-8')))