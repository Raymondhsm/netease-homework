# -*- coding: GBK -*-
from SimpleSocketHost import SimpleSocketHost
from Dispatcher import Dispatcher, Service
import config

class SimpleSocketServer(object):

    def __init__(self):
        super(SimpleSocketServer, self).__init__()
        self.host = SimpleSocketHost()
        self.dispatcher = Dispatcher()

    def StartServer(self):
        self.host.startup(8888)

    def HandleNewClient(self, hid):
        sid = self.GenerateSid(hid)
        svc = self.GetAllService()
        self.dispatcher.register(sid,svc)

    def GenerateSid(self, id):
        return id

    def GetAllService(self):
        return Service()

    def HandleClientLeave(self, hid):
        return

    def HandleData(self, hid, data):
        # client = self.host.getClient(hid)
        # return self.dispatcher.dispatch(data, client)
        return

	def tick(self):
		self.host.process()

        for type, hid, data in self.host.queue:
            if type == config.NET_CONNECTION_NEW :
                self.HandleNewClient(hid)

            elif type == config.NET_CONNECTION_LEAVE :
                self.HandleClientLeave(hid)
            
            elif type == config.NET_CONNECTION_DATA :
                self.HandleData(hid, data)


sm = SimpleSocketServer()

# server = SimpleSocketHost()
# server.startup(8765)
# prequeue = []
# while True:
#     server.process()
    # if prequeue != server.queue :
    #     print(server.queue)
    # prequeue = server.queue