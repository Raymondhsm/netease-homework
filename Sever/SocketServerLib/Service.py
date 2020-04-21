import os,sys
sys.path.append(os.path.realpath('./'))
import StartScene.Login as Login


COMMAND_REGISTER = 0x010000
COMMAND_LOGIN = 0x020000

service_dict = {
    COMMAND_LOGIN: Login.login,
    COMMAND_REGISTER: Login.register
}