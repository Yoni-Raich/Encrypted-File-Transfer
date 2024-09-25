from Crypto.PublicKey import RSA


key = RSA.generate(2048)
private_key = key.export_key()
public_key = key.publickey().export_key()

print(key.)
print(private_key)
print(public_key)

