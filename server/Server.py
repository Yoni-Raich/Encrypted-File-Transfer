import socket
import struct
import threading
from Protocol import Protocol
from RequestHandler import RequestHandler
from ClientManager import ClientManager

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
    def __init__(self, host='0.0.0.0'):
        self.host = host
        self.port = self.get_port_from_info()
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind((self.host, self.port))
        self.protocol = Protocol()
        self.client_manager = ClientManager()
        self.request_structure = None

    def get_port_from_info(self):
        try:
            f = open('port.info', 'r') 
            port = int(f.read().strip())
        
            #verify port
            if port < 1024 or port > 65535:
                raise ValueError("Invalid port number")
        except (ValueError, FileNotFoundError) as e:
            if isinstance(e, ValueError):
                print(f"Invalid port number: - Using default port 1256")
            else:
                print(f"Error reading port number from file - Using default port 1256")
            port = 1256

        return port
    
    def start(self):
        self.socket.listen()
        print(f"Server listening on {self.host}:{self.port}")
        while True:
            client_socket, address = self.socket.accept()
            client_thread = threading.Thread(target=self.handle_client, args=(client_socket, self.client_manager))
            client_thread.start()

    def handle_client(self, client_socket, client_manager):
        print(f"Accepted connection from {client_socket.getpeername()}")
        request_handler = RequestHandler()
        while True:
            try:
                request = receive_request(client_socket)
                if not request:
                    break

                received_request_struct = self.protocol.parse_request(request)
                request_handler.new_request(received_request_struct, client_manager)
                response = request_handler.create_response(client_socket)
                send_response(client_socket, response)

            except Exception as e:
                print(f"Error handling client: {e}")
                break

        client_socket.close()

if __name__ == "__main__":
    server = Server()
    server.start()
