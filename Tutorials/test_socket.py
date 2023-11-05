import ctypes

# Load thư viện C
client_lib = ctypes.CDLL("./libclient.so")

print("Gọi hàm C từ thư viện")

# Gọi hàm C từ thư viện
client_socket = client_lib.main()
if client_socket == -1:
    print("Không thể kết nối đến máy chủ")
else:
    print("Đã kết nối đến máy chủ")

# Sử dụng client_socket và socket API từ C để gửi và nhận dữ liệu
