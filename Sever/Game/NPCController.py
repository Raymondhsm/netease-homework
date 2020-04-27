import os,sys
sys.path.append(os.path.realpath('./'))
import EntityManager
from Config.Struct import newVector3,Vector3
from SocketServerLib import FightServer
from Config import config


class NPCController:
    def __init__(self):
        self.entityManager = EntityManager.entityManager
        self.npcInit = npcInit
        self.currLevel = -1
        self.currEnemies = []
        pass

    def Start(self, level = 0):
        for vector3 in npcInit[level]:
            entity = self.entityManager.RegisterNPC(vector3)
            FightServer.fightServer.boardcastCommand(config.COMMAND_NEW_ENTITY, entity.InfoDict())
            self.currEnemies.append(entity)

    def Process(self):
        if self.entityManager.getPlayerNum() == 0:
            return
        if len(self.currEnemies) == 0:
            self.currLevel += 1
            self.Start(self.currLevel)

npcInit = {
    0: [
        newVector3(-7.04,2.71,-16.96)
    ],

    1: [

    ],

    2: [

    ]
}

npcController = NPCController()