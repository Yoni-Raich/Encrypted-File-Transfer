import unittest
from unittest.mock import patch, MagicMock
from server import Server
from Protocol import Protocol
from RequestHandler import RequestHandler

class TestServer(unittest.TestCase):

    def setUp(self):
        self.server = Server(host='localhost', port=12345)

    def test_server_initialization(self):
        self.assertEqual(self.server.host, 'localhost')
        self.assertEqual(self.server.port, 12345)
        self.assertIsInstance(self.server.protocol, Protocol)
        self.assertIsInstance(self.server.requestHandler, RequestHandler)

    @patch('socket.socket')
    def test_server_start(self, mock_socket):
        mock_socket_instance = MagicMock()
        mock_socket.return_value = mock_socket_instance

        # Mock the accept method to return once and then raise an exception to stop the loop
        mock_socket_instance.accept.side_effect = [
            (MagicMock(), ('127.0.0.1', 54321)),
            Exception("Stop the loop")
        ]

        with self.assertRaises(Exception):
            self.server.start()

        mock_socket_instance.bind.assert_called_once_with(('localhost', 12345))
        mock_socket_instance.listen.assert_called_once()
        self.assertEqual(mock_socket_instance.accept.call_count, 2)

    @patch('threading.Thread')
    def test_handle_client(self, mock_thread):
        mock_client_socket = MagicMock()
        mock_client_socket.recv.side_effect = [
            b'data',  # First recv call
            b'',      # Second recv call to break the loop
        ]

        self.server.protocol.parse_request = MagicMock(return_value=(b'client_id', 1, 825, b'payload'))
        self.server.requestHandler.handle_register = MagicMock(return_value=b'response')

        self.server.handle_client(mock_client_socket)

        mock_client_socket.recv.assert_called()
        self.server.protocol.parse_request.assert_called_once_with(b'data')
        self.server.requestHandler.handle_register.assert_called_once_with(b'client_id', b'payload')
        mock_client_socket.send.assert_called_once_with(b'response')
        mock_client_socket.close.assert_called_once()

    def test_handle_client_exception(self):
        mock_client_socket = MagicMock()
        mock_client_socket.recv.side_effect = Exception("Test exception")

        self.server.handle_client(mock_client_socket)

        mock_client_socket.close.assert_called_once()

if __name__ == '__main__':
    unittest.main()