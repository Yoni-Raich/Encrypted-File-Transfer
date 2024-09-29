import socket
import struct
import threading
from Protocol import Protocol
from RequestHandler import RequestHandler
from CryptoManager import CryptoManager


def receive_full_message(client_socket, buffer_size=1024):
    data = b''
    while True:
        part = client_socket.recv(buffer_size)
        data += part
        if len(part) < buffer_size:
            break
    return data

class Server:
    def __init__(self, host='localhost', port=1234):
        self.host = host
        self.port = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind((self.host, self.port))
        self.protocol = Protocol()
        self.requestHandler = RequestHandler()

    def start(self):
        self.socket.listen()
        print(f"Server listening on {self.host}:{self.port}")
        while True:
            client_socket, address = self.socket.accept()
            client_thread = threading.Thread(target=self.handle_client, args=(client_socket,))
            client_thread.start()

    def handle_client(self, client_socket):
        while True:
            try:
                data = receive_full_message(client_socket)
                if not data:
                    break

                client_id, version, code, payload = self.protocol.parse_request(data)
                #print(f"client_id: {client_id}, version: {version}, code: {code}, \npayload: {payload}\n")
                if code == 825:  # Register request
                    response = self.requestHandler.handle_register(client_id, payload)
                elif code == 826:  # Send public key
                    response = self.requestHandler.handle_public_key(client_id, payload)
                elif code == 827:  # Reconnect
                    response = self.requestHandler.handle_reconnect(client_id, payload)
                elif code == 828:  # Send file
                    response = self.requestHandler.handle_file_transfer(client_id, payload)
                elif code in [900, 901, 902]:  # CRC responses
                    response = self.requestHandler.handle_crc_response(client_id, code, payload)
                else:
                    response = self.protocol.create_response(1607, client_id)  # General error
                
                message_length = len(response)
                client_socket.send(struct.pack('!I', message_length))
                client_socket.send(response)

            except Exception as e:
                print(f"Error handling client: {e}")
                break

        client_socket.close()
    
    
    
if __name__ == "__main__":
    server = Server()
    server.start()
