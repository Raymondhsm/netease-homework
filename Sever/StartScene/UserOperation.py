# -*- coding: GBK -*-
import os,sys
sys.path.append(os.path.realpath('./'))
import Database.Database as DB

def updateUserInfo(dataJson):
    db = DB.database
    sID = dataJson["sessionID"]
    (status, result) = db.select("user, userInfo on user.id=userInfo.id", "blood, bullet, level, experience","sessionID = '%s'"%sID)
    
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
    (status, result) = db.select("user, userInfo on user.id=userInfo.id", "blood, bullet","sessionID = '%s'"%sID)

    if status and result != []:
        return (True, result[0][0], result[0][1])
    else:
        return False

def updateFightResult(sID, bullet, blood, experience):
    db = DB.database
    (status1, result1) = db.select("user", "id","sessionID = '%s'"%sID)
    (status2, result2) = db.select("userInfo", "level,experience","id = '%s'"%result1[0][0])
    if status1 and status2:
        level = result2[0][0]
        exp = result2[0][1] + experience
        while exp > 500 + level * 100:
            exp -= 500 + level * 100
            level += 1
        col = {
            "blood": blood,
            "bullet": bullet,
            "level": level,
            "experience": exp
        }
        db.update("userInfo", col, "id = %s"%result1[0][0])

def addData(id):
    db = DB.database
    (status, result) = db.select("userInfo", "blood, bullet","id = %s"%id)
    if status:
        blood = result[0][0]
        bullet = result[0][1]

        blood = blood + 10 if blood <= 90 else 100
        bullet = bullet + 35 if bullet <= 175 else 210

        col = {
            "blood": blood,
            "bullet": bullet
        }
        db.update("userInfo", col, "id = %s"%id)

def findID(sID):
    db = DB.database
    (status, result) = db.select("user", "id","sessionID = '%s'"%sID)
    return result[0][0]