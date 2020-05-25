# -*- coding: GBK -*-

import Database.Database as DB
# db = DB.database
# (status, result) = db.select("user", "id","sessionID = '%s'"%'8bc061fa73934ae9924f6c4b6893682a')
# if status:
#     col = {
#         "blood": 99,
#         "bullet": 139,
#         "experience": 21
#     }
#     db.update("userInfo", col, "id = %s"%result[0][0])

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

updateFightResult('bc91907d09d84610b32b88c276824263', 123, 34, 8605)
        
