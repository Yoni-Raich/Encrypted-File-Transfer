import socket
import struct
from Crypto.PublicKey import RSA
from Crypto.Cipher import PKCS1_OAEP
from Crypto.Random import get_random_bytes

def send_request(client_socket, request):
    client_socket.sendall(request)

def receive_response(client_socket, buffer_size=1024):
    data = b''
    while True:
        part = client_socket.recv(buffer_size)
        data += part
        if len(part) < buffer_size:
            break
    return data

def create_register_request(client_id, name):
    version = 1
    code = 825
    payload = name.ljust(255, '\x00').encode('utf-8')
    payload_size = len(payload)
    header = struct.pack('16s B H I', client_id, version, code, payload_size)
    return header + payload

def create_public_key_request(client_id, name, public_key):
    version = 1
    code = 826
    payload = name.ljust(255, '\x00').encode('utf-8') + public_key
    payload_size = len(payload)
    header = struct.pack('16s B H I', client_id, version, code, payload_size)
    return header + payload

def parse_response(data):
    header = data[:24]
    client_id, version, code, payload_size = struct.unpack('16s B H I', header)
    payload = data[24:]
    return client_id, version, code, payload