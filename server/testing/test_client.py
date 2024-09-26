import socket
import struct
from Crypto.PublicKey import RSA
from Crypto.Cipher import PKCS1_OAEP
from Crypto.Random import get_random_bytes
from helper_functions import send_request, receive_response, create_register_request, create_public_key_request, parse_response

def test_client():
    # הגדרת פרטי השרת
    host = 'localhost'
    port = 1234

    # יצירת חיבור לשרת
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((host, port))

    # שלב 1: בקשה לרישום
    client_id = get_random_bytes(16)
    name = 'TestClient'
    register_request = create_register_request(client_id, name)
    send_request(client_socket, register_request)
    response = receive_response(client_socket)
    client_id, version, code, payload = parse_response(response)
    assert code == 1600, "Registration failed"

    # שלב 2: שליחת מפתח ציבורי
    rsa_key = RSA.generate(2048)
    public_key = rsa_key.publickey().export_key()
    public_key_request = create_public_key_request(client_id, name, public_key)
    send_request(client_socket, public_key_request)
    response = receive_response(client_socket)
    client_id, version, code, payload = parse_response(response)
    assert code == 1602, "Public key exchange failed"

    # פענוח מפתח AES שהתקבל
    encrypted_aes_key = payload[16:]
    cipher_rsa = PKCS1_OAEP.new(rsa_key)
    aes_key = cipher_rsa.decrypt(encrypted_aes_key)
    print("AES key received:", aes_key)

    # סגירת החיבור לשרת
    client_socket.close()

if __name__ == "__main__":
    test_client()