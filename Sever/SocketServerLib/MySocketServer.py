# -*- coding: GBK -*-
from SimpleSocketHost import SimpleSocketHost
import config
import Service
import struct
import json

class SimpleSocketServer(object):

    def __init__(self):
        super(SimpleSocketServer, self).__init__()
        self.host = SimpleSocketHost()

    def StartServer(self):
        self.host.startup(8765)

    def HandleNewClient(self, hid):
        # sid = self.GenerateSid(hid)
        # self.dispatcher.register(sid,svc)
        pass

    def GenerateSid(self, id):
        return id

    def HandleClientLeave(self, hid):
        return

    def HandleData(self, hid, data):
        try:
            command = struct.unpack(config.NET_HEAD_LENGTH_FORMAT, data[0:config.COMMAND_LENGTH_SIZE])[0]
            dataJson = json.loads(data[config.COMMAND_LENGTH_SIZE:])
            print("123")
            returnData = Service.service_dict[command](dataJson)
            print("456")
            self.send(hid, returnData)
        except:
            print("false")
            return None


    def Tick(self):
        self.host.process()
        while len(self.host.queue) > 0:
            type, hid, data = self.host.read()
            if type == -1: break

            if type == config.NET_CONNECTION_NEW:
                return self.HandleNewClient(hid)
            elif type == config.NET_CONNECTION_LEAVE:
                return self.HandleClientLeave(hid)
            elif type == config.NET_CONNECTION_DATA:
                return self.HandleData(hid,data)

    def send(self, hid, data):
        if isinstance(data,dict):
            sendData = json.dumps(data)
        else:
            sendData = data
        print(sendData)
        self.host.sendClient(hid, sendData)

    def boardcast(self, data):
        if isinstance(data,dict):
            sendData = json.dumps(data)
        else:
            sendData = data
        
        for client in self.host.clients:
            self.host.sendClient(client.hid, sendData)

socketServer = SimpleSocketServer()


