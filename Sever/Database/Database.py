import sqlite3

class Database:
    def __init__(self, file = "./Database/fps.db"):
        self.db = sqlite3.connect(file)
        self.cursor = self.db.cursor()

    def insert(self, table, data):
        if not isinstance(data,dict): 
            return False

        keyStr = ""
        valueStr= ""
        for key in data:
            keyStr += (key + ',')
            value = ('"%s"' % data[key]) if isinstance(data[key],str) else str(data[key])
            valueStr += (value + ',')

        keyStr = keyStr[:-1]
        valueStr = valueStr[:-1]
        
        sql = "insert into %s (%s) values (%s);" % (table, keyStr, valueStr)
        try:
            self.execute(sql)
            return True
        except:
            return False

    def select(self, table, cols, condition = ""):
        condi = "where %s" % condition if condition is not "" else ""
        sql = "select %s from %s %s;" % (cols, table, condi)

        try:
            result = self.execute(sql)
        except:
            return (False, "")

        resultList = []
        for row in result:
            resultList.append(row)
        return (True, resultList)

    def update(self, table, data, condition):
        cols = ""
        for key in data:
            value = ('"%s"' % data[key]) if isinstance(data[key],str) else str(data[key])
            cols += "%s = %s," % (key, value)
        cols = cols[:-1]

        sql = "update %s set %s where %s;" % (table, cols, condition)
        try:
            self.execute(sql)
            return True
        except:
            return False
        
    def execute(self, sql):
        data = self.cursor.execute(sql)
        self.db.commit()
        return data

database = Database()