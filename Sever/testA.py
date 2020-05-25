import sqlite3

db = sqlite3.connect("./Database/fps.db")
cursor = db.cursor()

sql = "select * from user;"
sql = "select * from userInfo;"
# sql = "delete from user where id = 1;"
# sql = "select blood, bullet, level, experience from user,userInfo where sessionID = 'cb7b5163083f4dff99fe3b3c69fc9f91'"
data = cursor.execute(sql)
db.commit()
for row in data:
    print(row)