import base64
import os
from Crypto.PublicKey import RSA
from Crypto.Cipher import AES, PKCS1_OAEP
from Crypto.Random import get_random_bytes
from Crypto.Util.Padding import pad, unpad
from cksum import memcrc as get_CRC
class CryptoManager:
    def __init__(self, public_key=None):
        self.private_key = None
        self.public_key = None
        self.aes_key = None

        if public_key:
            try:
                #public_key_bytes = base64.b64decode(public_key)
                self.public_key = RSA.import_key(public_key)
            except (ValueError, TypeError) as e:
                print(f"Invalid public key format: {e}")
    
    def get_encrypted_aes_key(self, public_key = None):
        if public_key:
            self.public_key = public_key
        
        return self.encrypt_rsa(self.aes_key)

    def generate_rsa_keys(self):
        key = RSA.generate(2048)  # Increased key size for better security
        self.private_key = key
        self.public_key = key.publickey()

    def encrypt_aes(self, data):
        if isinstance(data, str):
            data = data.encode('utf-8')
        
        padded_data = pad(data, AES.block_size)
        cipher_aes = AES.new(self.aes_key, AES.MODE_CBC)
        ciphertext = cipher_aes.encrypt(padded_data)
        
        return cipher_aes.iv + ciphertext

    def decrypt_aes(self, enc_data):
        iv = enc_data[:AES.block_size]
        ciphertext = enc_data[AES.block_size:]
        
        cipher_aes = AES.new(self.aes_key, AES.MODE_CBC, iv)
        padded_plaintext = cipher_aes.decrypt(ciphertext)
        plaintext = unpad(padded_plaintext, AES.block_size)
        
        return plaintext

    def encrypt_rsa(self, data):
        if isinstance(data, str):
            data = data.encode('utf-8')
        cipher_rsa = PKCS1_OAEP.new(self.public_key)
        encrypted_data = cipher_rsa.encrypt(data)
        return encrypted_data

    def decrypt_rsa(self, encrypted_data):
        cipher_rsa = PKCS1_OAEP.new(self.private_key)
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
    data = 'Hello my name is Yoni'

    enc_data = crypto_manager.encrypt_aes(data)
    print(enc_data)
    dec_data = crypto_manager.decrypt_aes(enc_data)
    print(dec_data)
