import os,sys
sys.path.append(os.path.realpath('./'))
from Config.Struct import Vector3

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
