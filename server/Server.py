import socket
import struct
import threading
from Protocol import Protocol
from RequestHandler import RequestHandler
from RequestStructur import RequestStructure

REQUEST_HEADER_SIZE = 23
RESPONSE_HEADER_SIZE = 7

def receive_request(client_socket):
    # Receive the fixed-size header
    header = client_socket.recv(REQUEST_HEADER_SIZE)
    if len(header) != REQUEST_HEADER_SIZE:
        return None

    # Extract payload size from the header
    payload_size = struct.unpack('!I', header[19:23])[0]

    # Receive the payload
    payload = b''
    remaining = payload_size
    while remaining > 0:
        chunk = client_socket.recv(min(remaining, 4096))
        if not chunk:
            return None
        payload += chunk
        remaining -= len(chunk)

    return header + payload

def send_response(client_socket, response):
    if len(response) < RESPONSE_HEADER_SIZE:
        raise ValueError("Invalid response size")

    # Send the fixed-size header
    client_socket.sendall(response[:RESPONSE_HEADER_SIZE])

    # Send the payload
    client_socket.sendall(response[RESPONSE_HEADER_SIZE:])

class Server:
    def __init__(self, host='localhost', port=1234):
        self.host = host
        self.port = port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind((self.host, self.port))
        self.protocol = Protocol()
        self.requestHandler = None
        self.request_structure = None

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
                request = receive_request(client_socket)
                if not request:
                    break

                self.receive_request = self.protocol.parse_request(request)
                self.requestHandler = RequestHandler(self.receive_request)
                response = self.requestHandler.create_response()
                send_response(client_socket, response)

            except Exception as e:
                print(f"Error handling client: {e}")
                break

        client_socket.close()

if __name__ == "__main__":
    server = Server()
    server.start()
