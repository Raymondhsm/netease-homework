import time
import SocketServerLib.MySocketServer as ss
import SocketServerLib.FightServer as fs
import Game.NPCController as npc

# socketServer = ss.socketServer
# socketServer.StartServer()
# while True:
#     currTime = time.time()
#     socketServer.Tick()
#     nextTime = time.time() - currTime
#     time.sleep(0.03 - nextTime)

socketServer = fs.fightServer
socketServer.StartServer()
while True:
    socketServer.Tick()
    npc.npcController.Process()