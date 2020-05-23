import os,sys
sys.path.append(os.path.realpath('./'))
from Entity import Entity, NPCEntity, PlayerEntity, MagicEntity
from Config.Struct import Vector3
from Config import config
import time

class EntityManager:
    def __init__(self):
        self.entities = {}
        self.entityPlayers = {}
        self.clientOwnEntities = {}
        self.entityMagic = {}
        self.eidIndex = 0
        self.updateTime = time.time()

    def RegisterPlayer(self, hid, entityInfo, publicID, privateID):
        entity = PlayerEntity(self.eidIndex)
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

    def RegisterReward(self, eType, pos):
        entity = Entity(self.eidIndex)
        entity.eType = eType
        entity.pos = pos
        self.entities[self.eidIndex] = entity
        self.eidIndex += 1
        return self.eidIndex-1

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

    def RegisterMagic(self):
        entity = MagicEntity(self.eidIndex)
        self.entities[self.eidIndex] = entity
        self.entityMagic[self.eidIndex] = entity
        self.eidIndex += 1
        return self.eidIndex-1

    def getPlayerNum(self):
        return len(self.entityPlayers)

    def register(self, hid):
        self.clientOwnEntities[hid] = []

    def registerEid(self):
        self.eidIndex += 1
        return self.eidIndex-1

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
                if entity.status == 1:
                    entity.status = 2
                    data.append((config.COMMAND_DEAD, {"eid": entity.eid}))
                if entity.status == 2:
                    continue
                if entity.prepareInfo(hnum) or time.time() - self.updateTime > 0.5:
                    entity.ProcessUpdateInfo()
                    data.append((config.COMMAND_UPDATE_ENTITY,entity.InfoDict()))
        if data != [] and time.time() - self.updateTime > 0.5:
            self.updateTime = time.time()
        return data

    def ProcessEntityHit(self, data):
        eid = data["eid"]
        entity = self.entities[eid]
        if entity.eType == config.ENTITY_PLAYER or entity.eType == config.ENTITY_ENEMY:
            entity.UpdateDamageInfo(data, len(self.entityPlayers))

    def ProcessNPCShoot(self, hid, data):
        eid = data["eid"]
        entity = self.entities[eid]
        if entity and entity.eType == config.ENTITY_ENEMY:
            entity.ProcessShoot(hid, len(self.entityPlayers))

    def ProcessPickUp(self, data):
        eid = data["eid"]
        Reid = data["rewardEid"]
        if eid in self.entities:
            self.entities[eid].PickUpReward(self.entities[Reid].eType)
            del self.entities[Reid]
            return True
        else:
            return False

    def ProcessShoot(self, hid):
        if hid in self.entityPlayers:
            return self.entityPlayers[hid].Shoot()
        else:
            return False

    def ProcessReload(self, hid):
        if hid in self.entityPlayers:
            self.entityPlayers[hid].Reload()

    def ProcessUseProp(self, hid, data):
        if hid in self.entityPlayers:
            self.entityPlayers[hid].UseProp(data)

    def ProcessMagicArrow(self):
        eid = self.RegisterMagic()
        return eid

    def ProcessMagicExplose(self, hid, data):
        eid = data['eid']
        if eid in self.entityMagic:
            pos = Vector3(data['pos'])
            self.entityMagic[eid].ProcessExplose(hid, pos, len(self.entityPlayers))

    def UpdateMagic(self):
        deleteMagic = []
        for key in self.entityMagic:
            magic = self.entityMagic[key]
            if magic.explosed:
                if magic.exploseTime + magic.exploseEffectTime < time.time():
                    deleteMagic.append(key)
                else:
                    continue
            elif len(magic.explosePosDict) != 0 and magic.endReceiptTime < time.time():
                magic.ProcessExplosePos()
            elif magic.bornTime + magic.lifeTime < time.time():
                print(magic.exploseTime + magic.exploseEffectTime < time.time())
                deleteMagic.append(key)
        
        for key in deleteMagic:
            del self.entityMagic[key]
            del self.entities[key]

                


entityManager = EntityManager()   