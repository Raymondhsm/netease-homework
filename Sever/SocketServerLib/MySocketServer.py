# -*- coding: GBK -*-
from SimpleSocketHost import SimpleSocketHost
import os,sys
sys.path.append(os.path.realpath('./'))
from Config import config
import Service
import struct
import json

class SimpleSocketServer(object):

    def __init__(self):
        super(SimpleSocketServer, self).__init__()
        self.host = SimpleSocketHost()
        self.sendQueue = []
        self.receiptQueue = []

    def StartServer(self):
        self.host.startup(8766)

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

            if command == config.COMMAND_ATTEND_GAME:
                (status, blood, bullet) = Service.service_dict[command](dataJson)
                if status:
                    self.sendQueue.append((dataJson['sessionID'], blood, bullet))
                self.send(hid, data[0:config.COMMAND_LENGTH_SIZE], {"status": status})
            else:
                returnData = Service.service_dict[command](dataJson)
                self.send(hid, data[0:config.COMMAND_LENGTH_SIZE], returnData)
        except:
            print("send return data failed")


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

    def send(self, hid, command, data):
        if isinstance(data,dict):
            sendData = json.dumps(data)
        else:
            sendData = data
        self.host.sendClient(hid, command + sendData)

    def boardcast(self, command, data):
        if isinstance(data,dict):
            sendData = json.dumps(data)
        else:
            sendData = data
        
        for client in self.host.clients:
            self.host.sendClient(client.hid, command + sendData)

socketServer = SimpleSocketServer()


