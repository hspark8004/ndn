import socket
from socket import error as SocketError
from time import sleep
import errno

HOST = '127.0.0.1'
PORT = 20000

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((HOST, PORT))
s.listen(5)
while True:
  conn, addr = s.accept()
  while True:
    try:
      data = conn.recv(1024)
      if not data:
        print("Server has not data")
        break;
      print("Server Receive: " + data)
      conn.sendall(data)
      print("Send all data" + data);
    except SocketError as e:
      print("Server error.")
      break
  print("Server closed.")
  conn.close()
s.close()
