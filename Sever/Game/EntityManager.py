import os,sys
sys.path.append(os.path.realpath('./'))
from Entity import Entity, NPCEntity, PlayerEntity, MagicEntity
from Config.Struct import Vector3, newVector3
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
        self.bulletOwner = {}

    def RegisterPlayer(self, hid, publicID, privateID, blood, bullet):
        entity = PlayerEntity(self.eidIndex,hid)
        entity.publicID = publicID
        entity.privateID = privateID
        entity.eType = config.ENTITY_PLAYER
        entity.pos = newVector3(len(self.entityPlayers),0,0)
        entity.direction = newVector3(0,0,1)
        entity.velocity = newVector3(0,0,0)
        entity.life = blood
        if bullet > entity.onceBullet:
            entity.totalBullet = bullet - entity.currBullet
        else:
            entity.currBullet = bullet
            entity.totalBullet = 0

        self.entities[self.eidIndex] = entity
        self.entityPlayers[hid] = entity
        self.clientOwnEntities[hid].append(self.eidIndex)
        self.eidIndex += 1
        return entity.InfoDict()

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

    def setBulletOwner(self, bulletID, eid):
        self.bulletOwner[bulletID] = eid

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
                    if entity.eType == config.ENTITY_PLAYER:
                        returnData = entity.ProcessEndGameData()
                        data.append((config.COMMAND_END_GAME, returnData, entity.hid))
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
            re = entity.UpdateDamageInfo(data, len(self.entityPlayers))
            if re != -1 and re in self.bulletOwner and entity.eType == config.ENTITY_ENEMY:
                beid = self.bulletOwner[re]
                if beid in self.entities and self.entities[beid].eType == config.ENTITY_PLAYER:
                    self.entities[self.bulletOwner[re]].damage += data["bulletDamage"]

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
                deleteMagic.append(key)
        
        for key in deleteMagic:
            del self.entityMagic[key]
            del self.entities[key]

                


entityManager = EntityManager()   