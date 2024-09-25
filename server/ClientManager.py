import uuid

class ClientManager:
    def __init__(self):
        self.clients = {}

    def add_client(self, client_id, name):
        client_id = str(uuid.uuid4())
        if client_id not in self.clients:
            self.clients[client_id] = {'name': name}
        else:
            raise     
        return client_id

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
    client = ClientManager()
    print(client.clients)
    client_id = client.add_client(0, 'test')
   
    print(client.clients)
   

