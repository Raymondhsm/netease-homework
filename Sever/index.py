# coding=utf-8

import socket
import struct
import SimpleSocketServer.config as config
print("start")
#   创建套接字
mySocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#   设置ip和端口
host = '127.0.0.1'
port = 8765
#   连接到服务器
mySocket.connect((host, port))
print("connected")
 
while True:
    print("read")
    msg = mySocket.recv(1024)
    a = struct.unpack()
    print("%s" % msg)
    print("read completed")
    if msg == b"EOF":
        break
    if msg == b"quit":
        mySocket.close()
        print("quit\n")
        exit()
 
    #   发送消息
    # msg = raw_input("send:")
    # data = struct.pack(config.NET_HEAD_LENGTH_FORMAT,len(msg)+4)+msg
    # mySocket.send(data)
    # print("send ok")
    # if msg == "EOF":
    #     break
    # if msg == "quit":
    #     mySocket.close()
    #     print("end")
    #     exit()