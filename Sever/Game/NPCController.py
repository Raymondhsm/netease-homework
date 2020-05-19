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
            if len(self.currEnemies) == 0 and self.currLevel < len(self.npcInit)-1:
                self.currLevel += 1
                self.Start(self.currLevel)

            deadEnemy = []
            for enemy in self.currEnemies:
                if enemy.status == 2:
                    deadEnemy.append(enemy)
                    continue
                self.Behavior(enemy)

            # del dead enemy
            for enemy in deadEnemy:
                if enemy.reward != -1:
                    data = {
                        "eid": self.entityManager.RegisterReward(enemy.reward, enemy.pos),
                        "Type": enemy.reward,
                        "pos": enemy.pos.toDict()
                    }
                    self.frameSendData.append((config.COMMAND_NEW_ENTITY, data))
                self.currEnemies.remove(enemy)
            
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
                reset = enemy.pos.distance(enemy.initPos) > 1
                if reset:
                    self.frameSendData.append((config.COMMAND_NPC_RESET, {"eid": enemy.eid, "pos": enemy.targetPos.toDict()}))
                else:
                    enemy.mode = NPCEntity.common
                    self.frameSendData.append((config.COMMAND_NPC_COMMON, {"eid": enemy.eid, "pos": enemy.targetPos.toDict()}))
        elif enemy.CheckToFar():
            enemy.mode = NPCEntity.reset
            self.frameSendData.append((config.COMMAND_NPC_RESET, {"eid": enemy.eid, "pos": enemy.targetPos.toDict()}))
        else:
            enemy.UpdateTarget(self.entityManager.entityPlayers)
            if enemy.CanShoot():
                data = {
                    "eid": enemy.eid,
                    "bulletEid": self.entityManager.registerEid(),
                    "pos": enemy.targetPos.toDict()
                }
                self.frameSendData.append((config.COMMAND_NPC_SHOOT, data))
            else:
                self.frameSendData.append((config.COMMAND_NPC_ATTACK, {"eid": enemy.eid, "pos": enemy.targetPos.toDict()}))



npcInit = {
    0: [
        {
            "pos": newVector3(-7.04,1.6,-16.96),
            "direction": newVector3(0,0,1),
            "discoverDistance": 10,
            "discoverAngle": 90,
            "toFar": 5,
            "reward": config.ENTITY_REWARD_MEDICINE
        },
        {
            "pos": newVector3(-8.74, 1.6, -14.59),
            "direction": newVector3(0,0,1),
            "discoverDistance": 10,
            "discoverAngle": 90,
            "toFar": 5,
            "reward": config.ENTITY_REWARD_BULLET 
        },
        {
            "pos": newVector3(-5.79, 1.6, -13.7),
            "direction": newVector3(0,0,1),
            "discoverDistance": 10,
            "discoverAngle": 90,
            "toFar": 5,
            "reward": -1
        }
    ],

    1: [
        {
            "pos": newVector3(39.3,1.6,-13.6),
            "direction": newVector3(0,0,1),
            "discoverDistance": 10,
            "discoverAngle": 90,
            "toFar": 5,
            "reward": config.ENTITY_REWARD_MEDICINE
        },
        {
            "pos": newVector3(44.02, 1.6, -13.6),
            "direction": newVector3(0,0,1),
            "discoverDistance": 10,
            "discoverAngle": 90,
            "toFar": 5,
            "reward": config.ENTITY_REWARD_BULLET 
        },
        {
            "pos": newVector3(40.51, 1.6, -17.04),
            "direction": newVector3(0,0,1),
            "discoverDistance": 10,
            "discoverAngle": 90,
            "toFar": 5,
            "reward": -1
        }
    ]
}

npcController = NPCController()