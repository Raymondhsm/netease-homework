import os,sys
sys.path.append(os.path.realpath('./'))
from Entity import Entity
from Config.Struct import Vector3

class EntityManager:
    def __init__(self):
        self.entities = {}
        self.eidIndex = 0

    def RegisterEntity(self, entityInfo, publicID, privateID):
        entity = Entity(self.eidIndex)
        entity.publicID = publicID
        entity.privateID = privateID
        entity.eType = entityInfo['entityCommand']
        entity.pos = Vector3(entityInfo['pos'])
        entity.direction = Vector3(entityInfo['direction'])
        entity.velocity = Vector3(entityInfo['velocity'])
        entity.life = entityInfo['life']

        self.entities[self.eidIndex] = entity
        self.eidIndex += 1
        return self.entities[self.eidIndex-1].InfoDict()

entityManager = EntityManager()   