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

from Server import Server

def main():
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

if __name__ == "__main__":
    main()
