import socket
import select

IP_SERVER = "127.0.0.1"

class SocketServer:
    def __init__(self, port):
        self.server_socket = None
        self.client_sockets = []
        self.port = port
        self.client = None
        self.target_client_port = None

        self.your_name = None

    def connect(self):
        try:
            print("=====Connect to server=====")
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            print("1. Kết nối máy chủ thành công.", IP_SERVER, self.port)
            self.server_socket.bind((IP_SERVER, self.port))
            print("2. Máy chủ đã được liên kết thành công.")
            self.server_socket.listen(5)
            print("3. Máy chủ đang lắng nghe...")
            self.server_socket.setblocking(0)  # Set the socket to non-blocking
            print("4. Máy chủ đang lắng nghe trên cổng", self.port)
        except Exception as e:
            print("Khởi tạo máy chủ thất bại:", str(e))

    def connect_to_target(self, port_target):
        print("=====Port target=====", port_target)
        try:
            self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client.connect((IP_SERVER, port_target))
            # self.server_socket.setblocking(0)  # Set the socket to non-blocking
            print("Ket noi may chu thanh cong:", port_target)
        except Exception as e:
            print("Ket noi máy chủ thất bại:", str(e)) 

    def check_for_new_connections(self):
        ready_to_read, _, _ = select.select([self.server_socket], [], [], 0)
        for sock in ready_to_read:
            # sock is not include accept function
            client_socket, client_address = sock.accept()
            # get port from client_address = ('127.0.0.1', 45840)            
            client_socket.setblocking(0)
            self.client_sockets.append(client_socket)
            print("Kết nối mới từ", client_address, client_address[1])
            # self.target_client_port = client_address[1]
            # connect to target

    def check_for_incoming_messages(self):
        if not self.client_sockets:
            return
        ready_to_read, _, _ = select.select(self.client_sockets, [], [], 0)
        for sock in ready_to_read:
            try:
                message = sock.recv(1024).decode()
                if message:
                    print("Nhận tin nhắn từ", sock.getpeername(), ":", message)
                    return message
                else:
                    # Connection is closed by the client
                    print("Kết nối bị ngắt từ", sock.getpeername())
                    self.client_sockets.remove(sock)
                    sock.close()
            except BlockingIOError:
                # No data available
                continue
            except Exception as e:
                print("Xảy ra lỗi:", str(e))
                self.client_sockets.remove(sock)
                sock.close()

    def send_message(self, message):
        for client_socket in self.client_sockets:
            client_socket.sendall(message.encode())

    def send_msg_target(self, message):
        if not self.client:
            self.connect_to_target(self.target_client_port)
        self.client.sendall(message.encode())

    def close_connection(self):
        for client_socket in self.client_sockets:
            client_socket.close()
        if self.server_socket:
            self.server_socket.close()
        print("Kết nối đã được đóng.")

# Usage Example
# server = SocketServer(12345)
# server.connect()
# server.run()
