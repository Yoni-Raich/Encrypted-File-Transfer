import sqlite3

class DatabaseManager:
    def __init__(self, db_name='server.db'):
        self.db_name = db_name
        self.conn = None

    def connect(self):
        self.conn = sqlite3.connect(self.db_name)

    def createTables(self):
        cursor = self.conn.cursor()
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS clients (
                id TEXT PRIMARY KEY,
                name TEXT,
                public_key BLOB,
                last_seen TIMESTAMP
            )
        ''')
        cursor.execute('''
            CREATE TABLE IF NOT EXISTS files (
                id TEXT PRIMARY KEY,
                client_id TEXT,
                file_name TEXT,
                file_path TEXT,
                verified BOOLEAN,
                FOREIGN KEY (client_id) REFERENCES clients (id)
            )
        ''')
        self.conn.commit()

    def insertClient(self, client_data):
        cursor = self.conn.cursor()
        cursor.execute('''
            INSERT OR REPLACE INTO clients (id, name, public_key, last_seen)
            VALUES (?, ?, ?, ?)
        ''', client_data)
        self.conn.commit()

    def insertFile(self, file_data):
        cursor = self.conn.cursor()
        cursor.execute('''
            INSERT INTO files (id, client_id, file_name, file_path, verified)
            VALUES (?, ?, ?, ?, ?)
        ''', file_data)
        self.conn.commit()

    def queryClient(self, client_id):
        cursor = self.conn.cursor()
        cursor.execute('SELECT * FROM clients WHERE id = ?', (client_id,))
        return cursor.fetchone()

    def queryFile(self, file_id):
        cursor = self.conn.cursor()
        cursor.execute('SELECT * FROM files WHERE id = ?', (file_id,))
        return cursor.fetchone()