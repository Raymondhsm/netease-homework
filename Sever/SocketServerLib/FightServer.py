# -*- coding: GBK -*-
import os,sys
sys.path.append(os.path.realpath('./'))
from SimpleSocketHost import SimpleSocketHost
from Config import config
from Game import EntityManager,NPCController
import struct,uuid,json,time

class FightServer(object):

    def __init__(self):
        super(FightServer, self).__init__()
        self.host = SimpleSocketHost()
        self.entityManager = EntityManager.entityManager
        self.npcController = NPCController.npcController
        self.clientDict = {}

    def StartServer(self):
        self.host.startup(8765)

    def HandleNewClient(self, hid):
        self.entityManager.register(hid)

        publicID = uuid.uuid4().hex
        privateID = uuid.uuid4().hex

        self.clientDict[hid] = (publicID, privateID)
        data = {
            "publicID" : publicID,
            "privateID" : privateID
        }
        self.send(hid, config.COMMAND_NEW_CLIENT, data)

        # create charactor
        for key in self.entityManager.entities:
            self.send(hid, config.COMMAND_NEW_ENTITY, self.entityManager.entities[key].InfoDict())
        

    def HandleClientLeave(self, hid):
        # delete own entity
        self.entityManager.deleteOwnEntity(hid)
        del self.clientDict[hid]

    def HandleData(self, hid, data):
        try:
            command = struct.unpack(config.NET_HEAD_LENGTH_FORMAT, data[0:config.COMMAND_LENGTH_SIZE])[0]
            dataJson = json.loads(data[config.COMMAND_LENGTH_SIZE:])
            if command < 0xff:
                self.boardcast(data)

            elif command == config.COMMAND_NEW_ENTITY:
                publicID, privateID = self.clientDict[hid]
                self.boardcastCommand(command, self.entityManager.RegisterEntity(hid, dataJson, publicID, privateID))
            
            elif command == config.COMMAND_UPDATE_ENTITY:
                self.entityManager.updateEntityInfo(hid, dataJson)

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
        self.EntityTick()
        self.NPCTick()

    def EntityTick(self):
        datalist = self.entityManager.ProcessEntityInfo()
        for data in datalist:
            data["time"] = time.time()
            self.boardcastCommand(config.COMMAND_UPDATE_ENTITY, data)

    def NPCTick(self):
        datalist = self.npcController.Process()
        if datalist:
            for data in datalist:
                data[1]["time"] = time.time()
                self.boardcastCommand(data[0], data[1])

    def send(self, hid, command, data):
        if isinstance(data,dict):
            sendData = json.dumps(data)
        else:
            sendData = data
        
        com = struct.pack(config.NET_HEAD_LENGTH_FORMAT, command)
        self.host.sendClient(hid, com + sendData)

    def boardcastCommand(self, command, data):
        if isinstance(data,dict):
            sendData = json.dumps(data)
        else:
            sendData = data
        
        com = struct.pack(config.NET_HEAD_LENGTH_FORMAT, command)
        
        for client in self.host.clients:
            if client is not None:
                self.host.sendClient(client.hid, com + sendData)

    def boardcast(self, allData):
        for client in self.host.clients:
            self.host.sendClient(client.hid, allData)

fightServer = FightServer()


