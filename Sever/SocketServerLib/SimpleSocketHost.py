# coding=utf-8
from Config import config
import socket
import time
import sys
from NetStream import NetStream

class SimpleSocketHost(object):
    def __init__ (self, timeout = config.NET_HOST_DEFAULT_TIMEOUT):
		super(SimpleSocketHost, self).__init__()

		self.host = 0
		self.state = config.NET_STATE_STOP
		self.clients = []
		self.index = 1
		self.queue = []
		self.count = 0
		self.sock = None
		self.port = 0
		self.timeout = timeout

		return

    def generateID(self):
		pos = -1
		for i in xrange(len(self.clients)):
			if self.clients[i] == None:
				pos = i
				break
		if pos < 0:
			pos = len(self.clients)
			self.clients.append(None)
		
		hid = (pos & config.MAX_HOST_CLIENTS_INDEX) | (self.index << config.MAX_HOST_CLIENTS_BYTES)
		
		self.index += 1
		if self.index >= config.MAX_HOST_CLIENTS_INDEX:
			self.index = 1

		return hid, pos

    # read event
    def read(self):
		if len(self.queue) == 0:
			return (-1, 0, '')	#type, hid, data
		event = self.queue[0]
		self.queue = self.queue[1:]
		return event

    # shutdown service
    def shutdown(self):
		if self.sock: 
			try: 
				self.sock.close()
			except: 
				pass	#should logging here
		self.sock = None

		self.index = 1
		for client in self.clients:
			if not client: 
				continue
			try: 
				client.close()
			except: 
				pass	#should logging here

		self.clients = []

		self.queue = []
		self.state = config.NET_STATE_STOP
		self.count = 0

		return

    # start listenning
    def startup(self, port = 0):
		self.shutdown()

		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
		try: 
			self.sock.bind(('127.0.0.1', port))
		except: 
			try: 
				self.sock.close()
			except: 
				pass	#should logging here
			return -1

		self.sock.listen(config.MAX_HOST_CLIENTS_INDEX + 1)
		self.sock.setblocking(0)
		self.port = self.sock.getsockname()[1]
		self.state = config.NET_STATE_ESTABLISHED

		return 0

    def getClient(self, hid):
		pos = hid & config.MAX_HOST_CLIENTS_INDEX
		if (pos < 0) or (pos >= len(self.clients)): 
			return -1, None
		
		client = self.clients[pos]
		if client == None: 
			return -2, None
		if client.hid != hid: 
			return -3, None

		return 0, client

    # close client
    def closeClient(self, hid):
		code, client = self.getClient(hid)
		if code < 0:
			return code

		client.close()
		return 0

    # send data to a certain client
    def sendClient(self, hid, data):
		code, client = self.getClient(hid)
		if code < 0:
			return code

		client.send(data)
		client.process()
		return 0

    def clientNodelay(self, hid, nodelay = 0):
        code, client = self.getClient(hid)
        if code < 0:
            return code
        return client.nodelay(nodelay)

    def handleNewClient(self, current):
		sock = None
		try:
			soc, remote = self.sock.accept()
			sock = soc._sock
			sock.setblocking(0)
		except: 
			pass	#log something

		if self.count > config.MAX_HOST_CLIENTS_INDEX:
			try: 
				sock.close()
			except: 
				pass	#log something
			sock = None

		if not sock:
			return

		hid, pos = self.generateID()
		client = NetStream()
		client.assign(sock)
		client.hid = hid
		client.active = current
		client.peername = sock.getpeername()
		self.clients[pos] = client
		self.count += 1
		self.queue.append((config.NET_CONNECTION_NEW, hid, repr(client.peername)))

		return

    def updateClients(self, current):
		for pos in xrange(len(self.clients)):
			client = self.clients[pos]
			if not client:
				continue

			client.process()
			while client.status() == config.NET_STATE_ESTABLISHED:
				data = client.recv()
				if data == '':
					break
				self.queue.append((config.NET_CONNECTION_DATA, client.hid, data))
				client.active = current

			timeout = current - client.active
			if (client.status() == config.NET_STATE_STOP) or (timeout >= self.timeout):
				self.queue.append((config.NET_CONNECTION_LEAVE, client.hid, ''))
				self.clients[pos] = None
				client.close()
				del client
				self.count -= 1
		return

    # update: process clients and handle accepting
    def process(self):
		current = time.time()
		if self.state != config.NET_STATE_ESTABLISHED:
			return 0

		self.handleNewClient(current)
		self.updateClients(current)
		
		return 0

