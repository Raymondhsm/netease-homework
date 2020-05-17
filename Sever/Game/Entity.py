import os,sys
sys.path.append(os.path.realpath('./'))
from Config.Struct import Vector3, newVector3
from Config import config

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
        if num == 1:
            self.life -= data["bulletDamage"]
        elif beid in self.damageInfo:
            if self.damageInfo[beid] + 1 >= num:
                self.life -= data["bulletDamage"]
                del self.damageInfo[beid]
            else:
                self.damageInfo[beid] += 1
        else:
            self.damageInfo[beid] = 1

        if self.life <= 0:
            self.status = 1

class PlayerEntity(Entity):
    def __init__(self, eid):
        Entity.__init__(self, eid)
        self.reward = {
            config.ENTITY_REWARD_MEDICINE : 0,
            config.ENTITY_REWARD_BULLET : 0
        }

    def PickUpReward(self, eType):
        self.reward[eType] += 1

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
        self.shootDict = {}
        self.shoot = False

    def CheckIfDiscoverPlayer(self, players):
        dis = 100000
        re = 0
        for key in players:
            player = players[key]
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

    def CheckToFar(self):
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

