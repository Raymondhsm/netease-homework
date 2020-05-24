# -*- coding: GBK -*-
import os,sys
sys.path.append(os.path.realpath('./'))
import Database.Database as DB

def updateUserInfo(dataJson):
    db = DB.database
    sID = dataJson["sessionID"]
    (status, result) = db.select("user, userInfo", "blood, bullet, level, experience","sessionID = '%s'"%sID)
    
    if not status:
        return False
    
    data = {
        "blood": result[0][0],
        "bullet": result[0][1],
        "level": result[0][2],
        "experience": result[0][3]
    }

    return data

def attendGame(dataJson):
    sID = dataJson['sessionID']
    db = DB.database
    (status, result) = db.select("user, userInfo", "blood, bullet","sessionID = '%s'"%sID)

    if status and result != []:
        return (True, result[0][0], result[0][1])
    else:
        return False
