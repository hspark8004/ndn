import socket
from time import sleep

HOST = '127.0.0.1'
PORT = 10000

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
s.sendall('Hello, world!')
data = s.recv(1024)
print(data)
#sleep(1)
#s.close()
