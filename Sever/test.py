import sqlite3

conn = sqlite3.connect('test.db')
print "Opened database successfully"
c = conn.cursor()
c.execute('''CREATE TABLE `user` (
            `id` INTEGER NOT NULL,
            `account` varchar(15) NOT NULL UNIQUE,
            `password` varchar(20) NOT NULL,
            `sessionId` varchar(45) DEFAULT NULL,
            `idTime` datetime DEFAULT CURRENT_TIMESTAMP,
            PRIMARY KEY (`id`)
            );''')
print "Table created successfully"

# data = c.execute("select * from user;")
# for row in data:
#     print(row)
# c.execute("insert into user (account,password) values ('netease','password');")
conn.commit()
conn.close()
