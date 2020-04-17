import os,sys
sys.path.append(os.path.realpath('./'))
import Database.Database as DB
import uuid

def login(data):
    db = DB.database
    (status, result) = db.select("user", "password", "account = '%s'"%data["account"])

    if status:
        pwd = result[0][0]
        if pwd == data["password"]:
            loginStatus = True
            sessionId = uuid.uuid4()
            print(sessionId)
            re = db.update("user",{"sessionId":sessionId},"account = '%s'"%data["account"])
            print(re)
            if re: 
                loginStatus = False
                sessionId = ""
        else:
            loginStatus = False
            sessionId = ""
    else:
        loginStatus = False
        sessionId = ""

    sendDict = {
        "loginStatus": loginStatus,
        "sessionId": sessionId
        }
    return sendDict

def register(data):
    pass

login({"account":"net","password":"123"})