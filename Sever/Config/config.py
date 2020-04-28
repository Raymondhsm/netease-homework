# -*- coding: utf-8 -*-

MSG_CS_LOGIN	= 0x1001
MSG_SC_CONFIRM	= 0x2001

MSG_CS_MOVETO	= 0x1002
MSG_SC_MOVETO	= 0x2002

NET_STATE_STOP	= 0				# state: init value
NET_STATE_CONNECTING	= 1		# state: connecting
NET_STATE_ESTABLISHED	= 2		# state: connected

NET_HEAD_LENGTH_SIZE	= 4		# 4 bytes little endian (x86)
NET_HEAD_LENGTH_FORMAT	= '<I'

NET_CONNECTION_NEW	= 0	# new connection
NET_CONNECTION_LEAVE	= 1	# lost connection
NET_CONNECTION_DATA	= 2	# data comming

NET_HOST_DEFAULT_TIMEOUT	= 70

MAX_HOST_CLIENTS_INDEX	= 0xffff
MAX_HOST_CLIENTS_BYTES	= 16

COMMAND_LENGTH_SIZE = 4
# COMMAND 字段
COMMAND_NEW_CLIENT = 0x030000

COMMAND_MOVE = 0x00
COMMAND_RUN = 0x01
COMMAND_JUMP = 0x02
COMMAND_SHOOT = 0x03
COMMAND_HIT = 0x04
COMMAND_RELOAD = 0x05

COMMAND_NPC_COMMON = 0x80
COMMAND_NPC_ATTACK = 0x81
COMMAND_NPC_RESET = 0x82

COMMAND_NEW_ENTITY = 0x0100
COMMAND_DEL_ENTITY = 0x0200
COMMAND_REGISTER_WITHOUT_EID = 0x0300
COMMAND_UPDATE_ENTITY = 0x0400

# 实体类型
ENTITY_PLAYER = 0x01000000
ENTITY_PLAYER_OTHER = 0x02000000
ENTITY_ENEMY = 0x03000000
ENTITY_BULLET = 0x04000000