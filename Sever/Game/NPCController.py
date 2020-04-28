import os,sys
sys.path.append(os.path.realpath('./'))
import EntityManager
from Entity import NPCEntity
from Config.Struct import newVector3,Vector3
from Config import config
import time


class NPCController:
    def __init__(self):
        self.entityManager = EntityManager.entityManager
        self.npcInit = npcInit
        self.currLevel = -1
        self.currEnemies = []

        self.frameSendData = []
        self.frameInterval = 0.1
        self.lastFrame = time.time()
        pass

    def Start(self, level = 0):
        for vector3 in npcInit[level]:
            entity = self.entityManager.RegisterNPC(vector3)
            self.frameSendData.append((config.COMMAND_NEW_ENTITY, entity.InfoDict()))
            self.currEnemies.append(entity)

    def Process(self):
        if time.time() - self.lastFrame > self.frameInterval:
            self.frameSendData = []
            if self.entityManager.getPlayerNum() == 0:
                return []
            if len(self.currEnemies) == 0:
                self.currLevel += 1
                self.Start(self.currLevel)

            for enemy in self.currEnemies:
                self.Behavior(enemy)
            self.lastFrame = time.time()
            return self.frameSendData
        else:
            return []

    def Behavior(self, enemy):
        if enemy.mode != NPCEntity.attack:
            if enemy.mode == NPCEntity.reset:
                reset = enemy.pos.distance(enemy.initPos) > 1
                if reset:
                    self.frameSendData.append((config.COMMAND_NPC_RESET, {"eid": enemy.eid, "pos": enemy.targetPos.toDict()}))
                else:
                    enemy.mode = NPCEntity.common
                    self.frameSendData.append((config.COMMAND_NPC_COMMON, {"eid": enemy.eid, "pos": enemy.targetPos.toDict()}))
            elif enemy.CheckIfDiscoverPlayer(self.entityManager.entityPlayers):
                enemy.mode = NPCEntity.attack
                self.frameSendData.append((config.COMMAND_NPC_ATTACK, {"eid": enemy.eid, "pos": enemy.targetPos.toDict()}))
            else:
                self.frameSendData.append((config.COMMAND_NPC_COMMON, {"eid": enemy.eid, "pos": enemy.targetPos.toDict()}))
        elif enemy.CheckToFar():
            enemy.mode = NPCEntity.reset
            self.frameSendData.append((config.COMMAND_NPC_RESET, {"eid": enemy.eid, "pos": enemy.targetPos.toDict()}))
        else:
            enemy.UpdateTarget(self.entityManager.entityPlayers)
            self.frameSendData.append((config.COMMAND_NPC_ATTACK, {"eid": enemy.eid, "pos": enemy.targetPos.toDict()}))



npcInit = {
    0: [
        {
            "pos": newVector3(-7.04,2.71,-16.96),
            "direction": newVector3(0,0,1),
            "discoverDistance": 10,
            "discoverAngle": 90,
            "toFar": 5
        }
    ],

    1: [

    ],

    2: [

    ]
}

npcController = NPCController()