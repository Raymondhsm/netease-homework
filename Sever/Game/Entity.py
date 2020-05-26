import os,sys
sys.path.append(os.path.realpath('./'))
from Config.Struct import Vector3, newVector3
from Config import config
import time

class Entity:
    def __init__(self, eid):
        self.publicID = 0
        self.private = 0
        self.eid = eid
        self.eType = -1
        self.pos = Vector3()
        self.direction = Vector3()
        self.velocity = Vector3()
        self.life = 100
        self.status = 0

        self.updateInfo = {}
        self.damageInfo = {}

    def process(self, command, data):
        pass

    def InfoDict(self):
        data = {
            "publicID": self.publicID,
            "Type": self.eType,
            "eid": self.eid,
            "pos": self.pos.toDict(),
            "direction": self.direction.toDict(),
            "velocity": self.velocity.toDict(),
            "life": self.life
        }
        return data

    def UpdateInfo(self, hid, info):
        self.updateInfo[hid] = info

    def prepareInfo(self, num):
        return len(self.updateInfo) == num

    def ProcessUpdateInfo(self):
        if self.updateInfo == {}:
            return

        count = [0,0,0]
        value = [Vector3(), Vector3(), Vector3()]
        for key in self.updateInfo:
            pos = Vector3(self.updateInfo[key]["pos"])
            if value[0].likely(pos, 0.1):
                count[0] += 1
            elif count[0] > 0:
                count[0] -= 1
            else:
                count[0] = 1
                value[0] = pos

            dir = Vector3(self.updateInfo[key]["direction"])
            if value[1].likely(dir, 0.1):
                count[1] += 1
            elif count[1] > 0:
                count[1] -= 1
            else:
                count[1] = 1
                value[1] = dir

            vel = Vector3(self.updateInfo[key]["velocity"])
            if value[2].likely(vel, 0.1):
                count[2] += 1
            elif count[2] > 0:
                count[2] -= 1
            else:
                count[2] = 1
                value[2] = vel

        self.pos = value[0]
        self.direction = value[1]
        self.velocity = value[2]

        self.updateInfo = {}

    def UpdateDamageInfo(self, data, num):
        if self.status > 0:
            return

        beid = data["bulletEid"]
        re = -1
        if num == 1:
            self.life -= data["bulletDamage"]
            re = beid
        elif beid in self.damageInfo:
            if self.damageInfo[beid] + 1 >= num:
                self.life -= data["bulletDamage"]
                del self.damageInfo[beid]
                re = beid
            else:
                self.damageInfo[beid] += 1
        else:
            self.damageInfo[beid] = 1

        if self.life <= 0:
            self.life = 0
            self.status = 1

        return re

class PlayerEntity(Entity):
    def __init__(self, eid, hid):
        Entity.__init__(self, eid)
        self.hid = hid
        self.reward = {
            config.ENTITY_REWARD_MEDICINE : 0,
            config.ENTITY_REWARD_BULLET : 0
        }
        self.onceBullet = 35
        self.currBullet = 35
        self.totalBullet = 105
        self.useBullet = 0
        self.nextMagicTime = time.time()
        self.magicInterval = 30
        self.damage = 0

    def UpdateInfo(self, hid, info):
        self.pos = Vector3(info['pos'])
        self.direction = Vector3(info['direction'])
        self.velocity = Vector3(info['velocity'])

    def prepareInfo(self, num):
        return True

    def ProcessUpdateInfo(self):
        return

    def ProcessMagic(self):
        if self.nextMagicTime <= time.time():
            self.nextMagicTime += self.magicInterval
            return True
        return False

    def PickUpReward(self, eType):
        self.reward[eType] += 1

    def Shoot(self):
        if self.currBullet > 0:
            self.currBullet -= 1
            self.useBullet += 1
            return True
        else:
            return False

    def Reload(self):
        use = self.onceBullet - self.currBullet
        if use > self.totalBullet:
            self.currBullet = self.currBullet + self.totalBullet
            self.totalBullet = 0
        else:
            self.currBullet += use
            self.totalBullet -= use 

    def InfoDict(self):
        data = Entity.InfoDict(self)
        data['totalBullet'] = self.totalBullet
        data['propMedicine'] = self.reward[config.ENTITY_REWARD_MEDICINE]
        data['propBullet'] = self.reward[config.ENTITY_REWARD_BULLET]
        return data
    
    def UseProp(self, data):
        type = data['type']
        if self.reward[type] > 0:
            if type == config.ENTITY_REWARD_MEDICINE:
                self.life = 100 if self.life >80 else self.life + 20
            elif type == config.ENTITY_REWARD_BULLET:
                self.totalBullet += 35
            self.reward[type] -= 1

    def ProcessEndGameData(self):
        if self.status != 2:
            return
        data = {
            "eid": self.eid,
            "useBullet": self.useBullet,
            "leftBullet": self.totalBullet + self.currBullet,
            "leftLife": self.life,
            "damage": self.damage,
            "experience": self.damage * 2
        }
        return data

class NPCEntity(Entity):
    common = 0
    attack = 1
    reset = 2

    def __init__(self, eid, init):
        Entity.__init__(self,eid)
        self.mode = NPCEntity.common
        self.targetPos = init["pos"]
        self.initPos = init["pos"]
        self.discoverDistance = init["discoverDistance"]
        self.discoverAngle = init["discoverAngle"]
        self.toFar = init["toFar"]
        self.reward = init["reward"]
        self.EnemyType = init["type"]
        self.shootDict = {}
        self.shoot = False
        self.dizzy = False

    def CheckIfDiscoverPlayer(self, players):
        dis = 100000
        re = 0
        for key in players:
            player = players[key]
            if player.status > 0:
                continue
            distance = self.pos.distance(player.pos)
            if distance > self.discoverDistance:
                continue
            
            angle = self.direction.angle(player.pos - self.pos)
            if angle > self.discoverAngle:
                continue

            if distance < dis:
                re = player.pos
                dis = distance

        if re:
            self.targetPos = re
            return True
        else:
            self.targetPos = self.initPos
            return False

    def CheckToFar(self, players):
        re = -1
        for key in players:
            if players[key].status == 0:
                re = 0
                break
        if re == -1:
            return True

        tofar = self.pos.distance(self.initPos) > self.toFar 
        if tofar:
            self.targetPos = self.initPos
            return True
        else:
            return False 

    def UpdateTarget(self, players):
        dis = 100000
        for key in players:
            player = players[key]
            if player.status > 0:
                continue
            distance = self.pos.distance(player.pos)
            if distance < dis:
                re = player.pos
                dis = distance

        if re:
            self.targetPos = re

    def ProcessShoot(self, hid, num):
        self.shootDict[hid] = True
        if len(self.shootDict) == num:
            self.shoot = True
            self.shootDict.clear()

    def CanShoot(self):
        status = self.shoot
        self.shoot = False
        return status

    def UpdateDamageInfo(self, data, num):
        if self.mode == NPCEntity.reset:
            return 
        return Entity.UpdateDamageInfo(self,data,num)

    def InfoDict(self):
        data = Entity.InfoDict(self)
        data['EnemyType'] = self.EnemyType
        return data

class MagicEntity(Entity):
    def __init__(self, eid):
        Entity.__init__(self, eid)
        self.explosed = False
        self.explosePos = newVector3(0,0,0)
        self.explosePosDict = {}
        self.bornTime = time.time()
        self.exploseTime = time.time()
        self.endReceiptTime = time.time()
        self.lifeTime = 20
        self.exploseEffectTime = 3
        self.exploseDistance = 3

    def ProcessExplose(self, hid, pos, num):
        if len(self.explosePosDict) == 0 and num != 1:
            self.explosePosDict[hid] = pos
            self.endReceiptTime = time.time() + 1
        elif len(self.explosePosDict) == 0 and num == 1:
            self.explosed = True
            self.explosePos = pos
            self.exploseTime = time.time()
        else:
            if time.time() > self.endReceiptTime or self.explosed:
                return
            self.explosePosDict[hid] = pos
            if len(self.explosePosDict) == num:
                self.ProcessExplosePos()
                
    
    def ProcessExplosePos(self):
        if len(self.explosePosDict) == 0:
            return
        self.explosed = True
        self.exploseTime = time.time()
        for key in self.explosePosDict:
            self.explosePos.add(self.explosePosDict[key])
        self.explosePos.division(len(self.explosePosDict))

