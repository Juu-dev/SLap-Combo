import random
import ctypes
import socket
import sys
from constant import *

PORT_SERVER = 5002
# IP_SERVER = "192.168.31.90" => server ip lan
IP_SERVER = "127.0.0.1"

class SocketClient:
    def __init__(self):
        self.client_socket = None
        self.username = None

    def connect(self):
        try:
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client_socket.connect((IP_SERVER, PORT_SERVER))
            print("Kết nối đến máy chủ thành công.")
        except Exception as e:  
            print("Kết nối thất bại:", str(e))

    def login(self, username, password):
        self.port_random = random.randint(5100, 6000)
        try:
            self.client_socket.sendall(f"LOGIN:{username}:{password}:{self.port_random}".encode())
            data = self.client_socket.recv(1024).decode()

            if data == "OK":
                self.username = username
                print("Đăng nhập thành công.")
                return 0
            return -1
        except Exception as e:
            print("Đăng nhập thất bại:", str(e))

    def register(self, username, password, confirm_password):
        try:
            self.client_socket.sendall(f"REGISTER:{username}:{password}:{confirm_password}".encode())
            data = self.client_socket.recv(1024).decode()
            return data
        except Exception as e:
            print("Đăng ký thất bại:", str(e))

    def logout(self):
        try:
            self.client_socket.sendall(f"LOGOUT:".encode())
            data = self.client_socket.recv(1024).decode()
            return data
        except Exception as e:
            print("Đăng xuất thất bại:", str(e))

    def update_state(self, state):
        try:
            self.client_socket.sendall(f"UPDATE_STATE:{self.username}:{state}".encode())
            data = self.client_socket.recv(1024).decode()
            return data
        except Exception as e:
            print("Cập nhật trạng thái thất bại:", str(e))

    def get_list_user(self):
        try:
            self.client_socket.sendall(f"GET_LIST_USER:".encode())
            data = self.client_socket.recv(1024).decode()
            print("===data===:", data)
            return data
        except Exception as e:
            print("Lấy danh sách người dùng thất bại:", str(e))

    def send_invite(self, username):
        try:
            self.client_socket.sendall(f"SEND_INVITE:{username}".encode())
            data = self.client_socket.recv(1024).decode()
            return data
        except Exception as e:
            print("Gửi lời mời thất bại:", str(e))

    def add_history(self, username, winner, loser, time):
        try:
            self.client_socket.sendall(f"ADD_HISTORY:{username}:{winner}:{loser}:{time}".encode())
            data = self.client_socket.recv(1024).decode()
            return data
        except Exception as e:
            print("Thêm lịch sử thất bại:", str(e))

    def get_history(self, username):
        try:
            self.client_socket.sendall(f"GET_HISTORY:{username}".encode())
            data = self.client_socket.recv(1024).decode()
            return data
        except Exception as e:
            print("Lấy lịch sử thất bại:", str(e))

    def close(self):
        if self.client_socket:
            self.client_socket.close()
            print("Đã đóng kết nối socket.")