import os,sys
sys.path.append(os.path.realpath('./'))
import StartScene.Login as Login
import StartScene.UserOperation as User
import Config.config as config


service_dict = {
    config.COMMAND_LOGIN: Login.login,
    config.COMMAND_REGISTER: Login.register,
    config.COMMAND_UPDATE_INFO: User.updateUserInfo,
    config.COMMAND_ATTEND_GAME: User.attendGame
}