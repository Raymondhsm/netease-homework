# -*- coding: GBK -*-
from SimpleSocketHost import SimpleSocketHost
from Dispatcher import Dispatcher, Service
import config
import json

class SimpleSocketServer(object):

    def __init__(self):
        super(SimpleSocketServer, self).__init__()
        self.host = SimpleSocketHost()
        self.dispatcher = Dispatcher()

    def StartServer(self):
        self.host.startup(8765)

    def HandleNewClient(self, hid):
        sid = self.GenerateSid(hid)
        svc = self.GetAllService()
        self.dispatcher.register(sid,svc)

    def GenerateSid(self, id):
        return id

    def GetAllService(self):
        return Service()

    def HandleClientLeave(self, hid):
        return

    def HandleData(self, hid, data):
        try:
            dataJson = json.loads(data)
        except:
            return None

        return dataJson

	def tick(self):
		self.host.process()

        while len(self.host.queue) > 0:
            type, hid, data = self.host.read()
            if type == -1: break

            if type == config.NET_CONNECTION_NEW:
                self.HandleNewClient(hid)
            elif type == config.NET_CONNECTION_LEAVE:
                self.HandleClientLeave(hid)
            elif type == config.NET_CONNECTION_DATA:
                self.HandleData(hid,data)

    def send(self, hid, data):
        if isinstance(data,dict):
            sendData = json.dumps(data)
        else:
            sendData = data
        self.host.sendClient(hid, sendData)

    def boardcast(self, data):
        if isinstance(data,dict):
            sendData = json.dumps(data)
        else:
            sendData = data
        
        for client in self.host.clients:
            self.host.sendClient(client.hid, sendData)


# sm = SimpleSocketServer()
# import time
# import NetStream
# import struct

# server = SimpleSocketHost()
# server.startup(8765)
# prequeue = []

# a=0
# while True:
#     server.process()
#     # if prequeue != server.queue :
#     # print(server.queue)
#         # prequeue = server.queue
#     # print(server.clients)
#     if len(server.clients) > 0 and server.clients[0] is not None :
#     # print(server.queue)
#         msg = str(a) + "hehehaha{...}"
#         # for i in range(3):
#         server.sendClient(server.clients[0].hid,msg)
#         a+=1
#         # print(server.clients[0].hid)
#         time.sleep(0.03)