import uuid

class ClientManager:
    def __init__(self):
        self.clients = {}

    def add_client(self, name):
        client_id = uuid.uuid4()
        print(client_id)
        if client_id not in self.clients:
            self.clients[str(client_id)] = {'name': name}
        else:
            raise ValueError("Client ID already exists")
        return client_id.bytes

    def get_client(self, client_id):
        return self.clients.get(client_id)

    def update_client(self, client_id, **kwargs):
        if client_id in self.clients:
            self.clients[client_id].update(kwargs)
            return True
        return False

    def remove_client(self, client_id):
        if client_id in self.clients:
            del self.clients[client_id]
            return True
        return False

    def client_exists(self, client_id):
        return client_id in self.clients
    
if __name__ == "__main__":
    my_uuid = uuid.uuid4().hex
    uuid_bytes = my_uuid.bytes
    uuid_str = str(uuid.UUID(bytes=uuid_bytes))
    print(uuid_str)