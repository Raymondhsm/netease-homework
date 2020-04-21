class Vector3:
    def __init__(self, dict = {'x':0.0, 'y':0.0, 'z':0.0}):
        self.x = dict['x']
        self.y = dict['y']
        self.z = dict['z']

    def toDict(self):
        return {
            'x': self.x,
            'y': self.y,
            'z': self.z
        }

COMMAND_MOVE = 0x00
COMMAND_RUN = 0x01
COMMAND_JUMP = 0x02
COMMAND_SHOOT = 0x03
COMMAND_HIT = 0x04
COMMAND_RELOAD = 0x05

COMMAND_NEW_ENTITY = 0x0100

class Entity:
    def __init__(self, eid):
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
            "Type": self.eType,
            "eid": self.eid,
            "pos": self.pos.toDict(),
            "direction": self.direction.toDict(),
            "velocity": self.velocity.toDict(),
            "life": self.life
        }
        return data
