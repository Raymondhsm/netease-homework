# -*- coding: GBK -*-
from SimpleSocketHost import SimpleSocketHost
import config
from Entity import *
import struct
import json

class FightServer(object):

    def __init__(self):
        super(FightServer, self).__init__()
        self.host = SimpleSocketHost()
        self.entities = {}
        self.eidIndex = 0

    def StartServer(self):
        self.host.startup(8765)

    def HandleNewClient(self, hid):
        # sid = self.GenerateSid(hid)
        # self.dispatcher.register(sid,svc)
        pass

    def HandleClientLeave(self, hid):
        return

    def RegisterEntity(self, entityInfo):
        entity = Entity(self.eidIndex)
        entity.eType = entityInfo['entityCommand']
        entity.pos = Vector3(entityInfo['pos'])
        entity.direction = Vector3(entityInfo['direction'])
        entity.velocity = Vector3(entityInfo['velocity'])
        entity.life = entityInfo['life']

        self.entities[self.eidIndex] = entity
        self.eidIndex += 1
        return self.entities[self.eidIndex-1].InfoDict()

    def HandleData(self, hid, data):
        try:
            command = struct.unpack(config.NET_HEAD_LENGTH_FORMAT, data[0:config.COMMAND_LENGTH_SIZE])[0]
            dataJson = json.loads(data[config.COMMAND_LENGTH_SIZE:])
            if command < 10:
                self.boardcast(data)

            elif command == COMMAND_NEW_ENTITY:
                self.boardcastCommand(command, self.RegisterEntity(dataJson))

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

    def boardcastCommand(self, command, data):
        if isinstance(data,dict):
            sendData = json.dumps(data)
        else:
            sendData = data
        
        com = struct.pack(config.NET_HEAD_LENGTH_FORMAT, command)
        
        for client in self.host.clients:
            self.host.sendClient(client.hid, com + sendData)

    def boardcast(self, allData):
        for client in self.host.clients:
            self.host.sendClient(client.hid, allData)

fightServer = FightServer()


