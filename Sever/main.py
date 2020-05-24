import time
import SocketServerLib.MySocketServer as ss
import SocketServerLib.FightServer as fs

myServer = ss.socketServer
myServer.StartServer()
fightServer = fs.fightServer
fightServer.StartServer()
while True:
    myServer.Tick()
    fightServer.Tick()