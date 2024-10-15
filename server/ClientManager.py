import os
import uuid
import threading

class ClientManager:
    root_clients_folder = "clients"

    def __init__(self):
        self.clients = {}
        self.lock = threading.Lock()

    def add_client(self, name):
        with self.lock:
            if any(client['name'] == name for client in self.clients.values()):
                raise ValueError("Username already exists")
            
            client_id = uuid.uuid4()
            self.clients[str(client_id)] = {'name': name}
            return client_id.bytes

    def get_client(self, client_id):
        with self.lock:
            return self.clients.get(client_id)

    def update_client(self, client_id, **kwargs):
        with self.lock:
            if client_id in self.clients:
                self.clients[client_id].update(kwargs)
                return True
            return False

    def remove_client(self, client_id):
        with self.lock:
            if client_id in self.clients:
                del self.clients[client_id]
                return True
            return False

    def client_exists(self, client_id):
        with self.lock:
            return client_id in self.clients
    
    def get_client_folder(self, client_id):
        with self.lock:
            folder_name = self.clients[client_id]['name']
            folder_path = os.path.join(self.root_clients_folder, folder_name)
            self.clients[client_id]['folder_path'] = folder_path
            return folder_path
    
    def save_client_file(self, client_id, filename, content):
        folder_path = self.get_client_folder(client_id)
        if not os.path.exists(folder_path):
            os.makedirs(folder_path)

        file_path = os.path.join(folder_path, filename)
        with open(file_path, 'wb') as f:
            f.write(content)
        
        with self.lock:
            if 'files' in self.clients[client_id]:
                self.clients[client_id]['files'].append(file_path)
            else:
                self.clients[client_id]['files'] = [file_path]
        return file_path
    
if __name__ == "__main__":
    new_uuid = uuid.uuid4()
   
