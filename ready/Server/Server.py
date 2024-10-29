"""
Secure File Transfer Server

This server application provides a secure way to transfer files between clients and the server.
It implements a custom protocol for client registration, authentication, and file transfer.

Key features:
- Client registration and reconnection
- RSA encryption for key exchange
- AES encryption for file transfer
- CRC checksum for file integrity verification

Developer: Yoni Raich
Date: 10/15/2024
"""

import socket
import struct
import threading
import errno
import time
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
        try:
            while True:
                try:
                    request = receive_request(client_socket)
                    #time.sleep(10)
                    print("\n------------------------------------------------------------------------------------\n")
                    if not request:
                        print(f"Client {client_socket.getpeername()} disconnected")
                        break

                    received_request_struct = self.protocol.parse_request(request)
                    request_handler.new_request(received_request_struct, client_manager)
                    response, code = request_handler.create_response(client_socket)
                    if code == 901:
                        continue
                    
                    send_response(client_socket, response)

                    # Check if the response code requires immediate disconnection
                    if code in [1601, 1604, 1606, 1607]:
                        print(f"Sent response with code {code}. Expecting client to disconnect.")
                        break  # Exit the loop to close the connection

                except socket.error as e:
                    if e.errno in (errno.ECONNRESET, errno.ECONNABORTED, errno.WSAECONNRESET, errno.WSAECONNABORTED):
                        print(f"Client {client_socket.getpeername()} disconnected")
                    else:
                        print(f"Socket error with client {client_socket.getpeername()}: {e}")
                    break

        except Exception as e:
            print(f"Unexpected error handling client {client_socket.getpeername()}: {e}")
        finally:
            client_socket.close()
           

if __name__ == "__main__":
    print("=" * 50)
    print("Secure File Transfer Server")
    print("Version: 3.0")
    print("Developer: Yoni Raich")
    print("ID: 318327962")
    print("Date: 10/15/2024")
    print("=" * 50)
    print()
    server = Server()
    server.start()
