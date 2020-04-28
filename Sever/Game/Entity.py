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
        self.life = 0.0

        self.updateInfo = {}

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
            
        count = [0,0,0,0,0,0]
        value = [0,0,0, Vector3(), Vector3(), Vector3()]
        for key in self.updateInfo:
            life = self.updateInfo[key]["life"]
            if life == value[0]:
                count[0]+=1
            elif count[0] > 0:
                count[0] -= 1
            else:
                count[0] = 1
                value[0] = life
            
            pos = Vector3(self.updateInfo[key]["pos"])
            if value[3].likely(pos, 0.1):
                count[3] += 1
            elif count[3] > 0:
                count[3] -= 1
            else:
                count[3] = 1
                value[3] = pos

            dir = Vector3(self.updateInfo[key]["direction"])
            if value[4].likely(dir, 0.1):
                count[4] += 1
            elif count[4] > 0:
                count[4] -= 1
            else:
                count[4] = 1
                value[4] = dir

            vel = Vector3(self.updateInfo[key]["velocity"])
            if value[5].likely(vel, 0.1):
                count[5] += 1
            elif count[5] > 0:
                count[5] -= 1
            else:
                count[5] = 1
                value[5] = vel

        self.life = value[0]
        self.pos = value[3]
        self.direction = value[4]
        self.velocity = value[5]

        self.updateInfo = {}


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
