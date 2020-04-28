import math

class Vector3:
    def __init__(self, dict = {'x':0.0, 'y':0.0, 'z':0.0}):
        self.x = dict['x']
        self.y = dict['y']
        self.z = dict['z']

    def setValue(self, list = [0, 0, 0]):
        self.x = list[0]
        self.y = list[1]
        self.z = list[2]

    def toDict(self):
        return {
            'x': self.x,
            'y': self.y,
            'z': self.z
        }

    def __sub__(self, vector3):
        x = self.x - vector3.x
        y = self.y - vector3.y
        z = self.z - vector3.z
        v = Vector3()
        v.setValue([x,y,z])
        return v

    def likely(self, vector3, offset):
        if abs(self.x - vector3.x) < offset and abs(self.y - vector3.y) < offset and abs(self.z - vector3.z) < offset :
            return True
        else:
            return False

    def add(self, vector3):
        self.x += vector3.x
        self.y += vector3.y
        self.z += vector3.z

    def division(self, num):
        self.x /= num
        self.y /= num
        self.z /= num

    def distance(self, vector3):
        x2 = math.pow(self.x - vector3.x, 2)
        y2 = math.pow(self.y - vector3.y, 2)
        z2 = math.pow(self.z - vector3.z, 2)
        return math.sqrt(x2 + y2 + z2)

    def dot(self, vector3):
        x = self.x * vector3.x
        y = self.y * vector3.y
        z = self.z * vector3.z
        return x + y + z

    def length(self):
        x2 = math.pow(self.x, 2)
        y2 = math.pow(self.y, 2)
        z2 = math.pow(self.z, 2)
        return math.sqrt(x2 + y2 + z2)

    def angle(self, vector3):
        len = (self.length() * vector3.length())
        if len == 0:
            return 0
        else:
            cos = self.dot(vector3) / (self.length() * vector3.length())
            return math.acos(cos)

def newVector3(x,y,z):
    vector3 = Vector3()
    vector3.setValue([x,y,z])
    return vector3