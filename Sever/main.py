import time
import SocketServerLib.MySocketServer as ss

socketServer = ss.socketServer
socketServer.StartServer()
while True:
    socketServer.Tick()
    time.sleep(0.03)