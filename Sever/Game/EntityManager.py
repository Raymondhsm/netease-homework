import os,sys
sys.path.append(os.path.realpath('./'))
from Entity import Entity, NPCEntity
from Config.Struct import Vector3
from Config import config
import time

class EntityManager:
    def __init__(self):
        self.entities = {}
        self.entityPlayers = {}
        self.clientOwnEntities = {}
        self.eidIndex = 0
        self.updateTime = time.time()

    def RegisterEntity(self, hid, entityInfo, publicID, privateID):
        entity = Entity(self.eidIndex)
        entity.publicID = publicID
        entity.privateID = privateID
        entity.eType = entityInfo['entityCommand']
        entity.pos = Vector3(entityInfo['pos'])
        entity.direction = Vector3(entityInfo['direction'])
        entity.velocity = Vector3(entityInfo['velocity'])
        entity.life = entityInfo['life']

        self.entities[self.eidIndex] = entity
        if entityInfo['entityCommand'] == config.ENTITY_PLAYER:
            self.entityPlayers[hid] = entity
        self.clientOwnEntities[hid].append(self.eidIndex)
        self.eidIndex += 1
        return self.entities[self.eidIndex-1].InfoDict()

    def RegisterNPC(self, init):
        entity = NPCEntity(self.eidIndex, init)
        entity.pos = init["pos"]
        entity.direction = init["direction"]
        entity.publicID = -1
        entity.privateID = -1
        entity.eType = config.ENTITY_ENEMY
        entity.life = 100

        self.entities[self.eidIndex] = entity
        self.eidIndex += 1
        return self.entities[self.eidIndex-1]

    def getPlayerNum(self):
        return len(self.entityPlayers)

    def register(self, hid):
        self.clientOwnEntities[hid] = []

    def registerEid(self):
        self.eidIndex += 1
        return self.eidIndex

    def deleteOwnEntity(self, hid):
        if hid in self.entityPlayers:
            del self.entityPlayers[hid]
        for index in self.clientOwnEntities[hid]:
            del self.entities[index]

    def updateEntityInfo(self, hid, data):
        if data["eid"] in self.entities:
            self.entities[data["eid"]].UpdateInfo(hid,data)

    def ProcessEntityInfo(self):
        hnum = len(self.clientOwnEntities)
        data = []
        for key in self.entities:
            entity = self.entities[key]
            if entity.eType == config.ENTITY_PLAYER or entity.eType == config.ENTITY_ENEMY:
                if entity.prepareInfo(hnum) or time.time() - self.updateTime > 0.5:
                    entity.ProcessUpdateInfo()
                    data.append(entity.InfoDict())
                    self.updateTime = time.time()
        return data

    def ProcessEntityHit(self, data):
        eid = data["eid"]
        entity = self.entities[eid]
        if entity.eType == config.ENTITY_PLAYER or entity.eType == config.ENTITY_ENEMY:
            entity.UpdateDamageInfo(data, len(self.entityPlayers))
                


entityManager = EntityManager()   