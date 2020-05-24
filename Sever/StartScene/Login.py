# -*- coding: GBK -*-
import os,sys
sys.path.append(os.path.realpath('./'))
import Database.Database as DB
import uuid
import time

def login(data):
    db = DB.database
    (status, result) = db.select("user", "password", "account = '%s'"%data["account"])

    if status:
        if result == []:
            loginStatus = False
            sessionId = "account wrong"
        else:
            pwd = result[0][0]
            if pwd == data["password"]:
                loginStatus = True
                sessionId = uuid.uuid4().hex
                idTime = time.strftime('%Y-%m-%d %H:%M:%S',time.gmtime(time.time()))
                re = db.update("user",{"sessionId":sessionId,"idTime":idTime},"account = '%s'"%data["account"])
                if not re: 
                    loginStatus = False
                    sessionId = "service wrong"
            else:
                loginStatus = False
                sessionId = "password wrong"
    else:
        loginStatus = False
        sessionId = "service wrong"

    sendDict = {
        "loginStatus": loginStatus,
        "sessionId": sessionId
        }
    return sendDict

def register(data):
    db = DB.database
    (status, result) = db.select("user", "password", "account = '%s'"%data["account"])

    registerStatus = False
    if status and result == []:
        info = {
            "account": data["account"],
            "password": data["password"]
        }
        re = db.insert("user",info)
        if re:
            (status, result) = db.select("user", "id", "account = '%s'"%data["account"])
            info = {
                "id": result[0][0]
            }
            re = db.insert("userInfo", info)

            registerStatus = True
            registerInfo = "success"
        else:
            registerInfo = "service wrong"
    else:
        registerInfo = "account exist"
    
    sendData = {
        "registerStatus": registerStatus,
        "registerInfo": registerInfo
        }
    return sendData
