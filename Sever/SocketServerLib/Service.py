import os,sys
sys.path.append(os.path.realpath('./'))
import StartScene.Login as Login
import StartScene.UserInfo as userInfo


COMMAND_REGISTER = 0x010000
COMMAND_LOGIN = 0x020000
COMMAND_UPDATE_INFO = 0x040000

service_dict = {
    COMMAND_LOGIN: Login.login,
    COMMAND_REGISTER: Login.register,
    COMMAND_UPDATE_INFO: userInfo.updateUserInfo
}