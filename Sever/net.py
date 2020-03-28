# -*- coding: GBK -*-

# http client
# ---------------------------------------------
# import urllib
# import urllib2

# loginBaiduUrl='http://localhost:8080/l'    #登陆百度空间的URL
# para={
#     'name':'xiaoming',
#     'id':13121,
#     'username':'XXXXXXXX',
#     'password':'XXXXXXXX',
#     'mem_pass':'on',  #勾选下次自动登陆
#     }
# postData=urllib.urlencode(para)
# req=urllib2.Request(loginBaiduUrl,postData)  #提供请求参数后request就是Post请求，非Get请求
# req.add_header('User-Agent','Mozilla/5.0 (compatible; MSIE 8.0;)')
# req.add_header('Content-Type','application/x-www-form-urlencoded')  #Post请求必要条件
# req.add_header('Cache-Control','no-cache')
# req.add_header('Accept','*/*')
# req.add_header('Connection','Keep-Alive')
# resp=urllib2.urlopen(req)
# respInfo=resp.read()
# print respInfo

import socket
print("程序开始")
#   创建套接字
mySocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#   设置ip和端口
host = socket.gethostname()
port = 8765
#   连接到服务器
mySocket.connect((host, port))
print("连接到服务器")
 
while True:
    #   接收消息
    print("----------------------读取:")# 不换行输出
    msg = mySocket.recv(1024)
    print("%s" % msg)
    print("读取完成")
    if msg == b"EOF":
        break
    if msg == b"quit":
        mySocket.close()
        print("程序结束\n")
        exit()
 
    #   发送消息
    msg = input("----------------------发送:")
    mySocket.send(msg.encode())
    print("发送完成")
    if msg == "EOF":
        break
    if msg == "quit":
        mySocket.close()
        print("程序结束\n")
        exit()
print("程序结束\n")
