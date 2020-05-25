# -*- coding: GBK -*-
from SimpleSocketHost import SimpleSocketHost
import os,sys
sys.path.append(os.path.realpath('./'))
from Config import config
import StartScene.UserOperation as User
import Service
import struct
import json, time

class SimpleSocketServer(object):

    def __init__(self):
        super(SimpleSocketServer, self).__init__()
        self.host = SimpleSocketHost()
        self.sendQueue = []
        self.receiptQueue = []
        self.nextAddTime = time.time()
        self.fightClient = {}
        self.onlineClient = {}

    def StartServer(self):
        self.host.startup(8766)

    def HandleNewClient(self, hid):
        pass

    def HandleClientLeave(self, hid):
        delete = []
        for key in self.onlineClient:
            if self.onlineClient[key] == hid:
                delete.append(key)
        for key in delete:
            del self.onlineClient[key]

    def HandleData(self, hid, data):
        try:
            command = struct.unpack(config.NET_HEAD_LENGTH_FORMAT, data[0:config.COMMAND_LENGTH_SIZE])[0]
            dataJson = json.loads(data[config.COMMAND_LENGTH_SIZE:])

            if command == config.COMMAND_ATTEND_GAME:
                (status, blood, bullet) = Service.service_dict[command](dataJson)
                if status:
                    self.sendQueue.append((dataJson['sessionID'], blood, bullet))
                self.send(hid, data[0:config.COMMAND_LENGTH_SIZE], {"status": status})
            elif command == config.COMMAND_CHECK_LOGIN:
                if dataJson["sessionID"] in self.fightClient:
                    status = True
                    self.onlineClient[User.findID(dataJson["sessionID"])] = hid
                    del self.fightClient[dataJson["sessionID"]]
                else:
                    status = False
                self.send(hid, data[0:config.COMMAND_LENGTH_SIZE], {"status": status})
            else:
                returnData = Service.service_dict[command](dataJson)
                self.send(hid, data[0:config.COMMAND_LENGTH_SIZE], returnData)
                if command == config.COMMAND_LOGIN and 'id' in returnData:
                    if returnData['id'] != -1:
                        self.onlineClient[returnData['id']] = hid
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

        self.ReceiptQueueTick()
        self.AddData()

    def ReceiptQueueTick(self):
        if self.receiptQueue == []:
            return

        for value in self.receiptQueue:
            self.fightClient[value[0]] = True
            User.updateFightResult(value[0], value[1], value[2], value[3])
        self.receiptQueue = []

    def AddData(self):
        if self.nextAddTime > time.time():
            return
        for id in self.onlineClient:
            User.addData(id)
        self.nextAddTime += 10

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


