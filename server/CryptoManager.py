import base64
import os
from Crypto.PublicKey import RSA
from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.Random import get_random_bytes
from cksum import memcrc as get_CRC
class CryptoManager:
    def __init__(self, public_key=None):
        self.private_key = None
        self.public_key = None
        self.aes_key = None

        if public_key:
            try:
                public_key_bytes = base64.b64decode(public_key)
                self.public_key = RSA.import_key(public_key_bytes)
            except (ValueError, TypeError) as e:
                print(f"Invalid public key format: {e}")
    
    def get_encrypted_aes_key(self, public_key = None):
        if public_key:
            self.public_key = public_key
        
        return self.encrypt_rsa(self.aes_key)

    def generate_rsa_keys(self):
        key = RSA.generate(1024)
        self.private_key = key.export_key()
        self.public_key = key.publickey().export_key()

    def encrypt_aes(self, data):
        cipher_aes = AES.new(self.aes_key, AES.MODE_CBC)
        ciphertext = cipher_aes.encrypt(data)
        return cipher_aes.iv + ciphertext

    def decrypt_aes(self, data):
        iv = data[:16]
        ciphertext = data[16:]
        cipher_aes = AES.new(self.aes_key, AES.MODE_CBC, iv)
        plaintext = cipher_aes.decrypt(ciphertext)
        return plaintext

    def encrypt_rsa(self, data):
        cipher_rsa = PKCS1_OAEP.new(self.public_key)
        encrypted_data = cipher_rsa.encrypt(data)
        return encrypted_data

    def decrypt_rsa(self, encrypted_data):
        private_key = RSA.import_key(self.private_key)
        cipher_rsa = PKCS1_OAEP.new(private_key)
        decrypted_data = cipher_rsa.decrypt(encrypted_data)
        return decrypted_data

    def generate_aes_key(self):
        self.aes_key = get_random_bytes(32)

    def get_CRC(self, data, decrypt = False):
        if decrypt:
            data = self.decrypt_aes(data)
        
        return get_CRC(data)
    
    
        

# Example usage
if __name__ == "__main__":
    crypto_manager = CryptoManager()
    crypto_manager.generate_rsa_keys()
    crypto_manager.generate_aes_key()
    enc_aes_key = crypto_manager.get_encrypted_aes_key()
    print(enc_aes_key)
