# coding=utf-8
class macros:
    def __init__(self, head, key, value):
        self.head = head
        self.key = key
        self.value = value


class PyMacroParser:

    HEADLIST = {"#else" : 0, "#endif" : 0, "#ifdef" : 1, "#ifndef" : 1, "#undef" : 1, "#define" : 2}
    FLOAT1 = ['f']

    def __init__(self):
        self.preDefineDict = {}
        self.loadList = []

    def load(self, f):
        _lineList = self.__fileToLineList(f)
        isAnnotation = False
        for _line in _lineList:
            # 空行
            if _line == "":
                continue

            restr = ""
            preC = ''
            initializerCounter = 0
            isInitializer, isDouble, isSingle = False, False, False
            info = {
                "head": None,
                "key": None,
                "value": None
            }

            def storeData(_data):
                _data = _data.strip()
                if info["head"] is None and _data in self.HEADLIST:
                    info["head"] = _data
                elif info["head"] is not None and info["key"] is None:
                    if self.HEADLIST[info["head"]] < 1:
                        raise Exception("error define, extra key {}".format(_data))
                    else:
                        info["key"] = _data
                elif info["head"] is not None and info["key"] is not None and info["value"] is None:
                    if self.HEADLIST[info["head"]] < 2:
                        raise Exception("error define, extra value {}".format(_data))
                    else:
                        info["value"] = self.__analysisValue(_data)
                else:
                    # throw exception
                    raise Exception("error define, extra data {}".format(_data))

            for c in _line:
                if (c == " " or c == "\n" or c == "\t") and not isAnnotation and not isDouble and not isInitializer and not isSingle:
                    if restr == "":
                        continue
                    else:
                        storeData(restr)
                        restr = ""

                elif c != " " and not isAnnotation and not isDouble and not isInitializer and not isSingle:
                    if c == "/" and preC == "/":
                        if restr != '/':
                            storeData(restr[:-1])
                        restr = ""
                        break
                    elif c == "*" and preC == "/":
                        isAnnotation = True
                        if restr != '/':
                            storeData(restr[:-1])
                        restr = ""
                    elif c == "\"":
                        isDouble = True
                        restr += c
                    elif c == "\'":
                        isSingle = True
                        restr += c
                    elif c == '{':
                        isInitializer = True
                        initializerCounter += 1
                        restr += c
                    else:
                        restr += c

                elif isAnnotation:
                    if c == "/" and preC == "*":
                        isAnnotation = False
                    else:
                        pass

                elif isDouble:
                    if c == "\"":
                        restr += c
                        isDouble = False
                    else:
                        restr += c

                elif isSingle:
                    if c == '\'':
                        restr += c
                        isSingle = False
                    else:
                        restr += c
                
                elif isInitializer:
                    if c == '}':
                        restr += c
                        initializerCounter -= 1
                        if initializerCounter == 0:
                            isInitializer = False
                            storeData(restr)
                            restr = ""
                    elif c == '{':
                        restr += c
                        initializerCounter += 1
                    else:
                        restr += c

                preC = c

            if info["head"] is not None:
                self.loadList.append(macros(info["head"], info['key'], info['value']))

    def preDefine(self, s):
        self.preDefineDict = {}

        if s == "":
            return
        else:
            slist = s.split(';')
            for sl in slist:
                self.preDefineDict[sl.strip()] = None

    def dumpDict(self):
        reDict = self.__analysisMacros()
        print(reDict)

    def dump(self, f):
        reDict = self.__analysisMacros()
        head = "#define"

        file = open(f, "w")
        for re in reDict.keys():
            key = re
            value = reDict[re]
            if value == None:
                file.writelines("{} {}\n".format(head, key))
            else:
                newValue = self.__anyToString(value)
                file.writelines("{} {} {}\n".format(head, key, newValue))
        file.close()
    
    def __tupleToInitializer(self, tup):
        re = "{"
        for t in tup:
            if t is tuple:
                re += self.__tupleToInitializer(t)
            else:
                re += str(self.__anyToString(t))
            re += ", "
        re = (re if len(re) < 3 else re[:-2]) + "}"
        return re

    def __anyToString(self, value):
        if isinstance(value, tuple):
            newValue = self.__tupleToInitializer(value)
        elif isinstance(value, unicode):
            newValue = value.encode("utf-8")
            newValue = "L\"" + newValue + "\""
        elif isinstance(value, str):
            newValue = "\"" + value + "\""
        elif isinstance(value, bool):
            if value:
                newValue = "true"
            else:
                newValue = "false"
        else:
            newValue = value
        return newValue

    def __analysisMacros(self):
        macrosDict = self.preDefineDict.copy()
        ifStack = []

        def top(stack):
            return False if len(stack) < 1 else stack[len(stack)-1]

        def underTop(stack):
            return False if len(stack) <= 1 else stack[len(stack)-2]

        for _macros in self.loadList:
            if _macros.head == "#define" and (len(ifStack) == 0 or top(ifStack)):
                macrosDict[_macros.key] = _macros.value

            elif _macros.head == "#undef" and (len(ifStack) == 0 or top(ifStack)):
                if _macros.key in macrosDict:
                    del macrosDict[_macros.key]

            elif _macros.head == "#ifdef" or _macros.head == "#ifndef":
                condition = _macros.key in macrosDict
                if _macros.head == "#ifndef":
                    condition = not condition
                if len(ifStack) == 0:
                    ifStack.append(condition)
                else:
                    ifStack.append(top(ifStack) and condition)

            elif _macros.head == "#else":
                if underTop(ifStack) or (len(ifStack) == 1 and top(ifStack)):
                    condition = ifStack.pop()
                    ifStack.append(not condition)
                elif len(ifStack) == 0:
                    # throw exception
                    raise Exception("extra #else")

            elif _macros.head == "#endif":
                if len(ifStack) > 0:
                    ifStack.pop()
                else:
                    # throw exception
                    raise Exception("extra #endif")

        return macrosDict

    def __fileToLineList(self, fileName):
        lineList = []

        f = open(fileName)
        line = f.readline()
        while line:
            lineList.append(line)
            line = f.readline()
        f.close()

        return lineList

    def __isChar(self, s):
        return len(s) >= 2 and s[0] == '\'' and s[-1] == '\''

    def __isStr(self, s):
        return len(s) >= 2 and s[0] == '\"' and s[-1] == '\"'

    def __isLStr(self, s):
        return len(s) >= 3 and s[:2] == "L\"" and s[-1] == '\"'

    def isFloat(self, s):
        floatHou = ['f', 'l']
        isf = False
        if s[-1].lower() in floatHou:
            isf = True
            newS = s[:-1]
        else:
            newS = s
        
        if isf or 'e' in newS or '.' in newS:
            try:
                re = float(newS)
                return True, re
            except Exception:
                return False, None
        else:
            return False, None

    def isInt(self, s):
        int1 = ['u', 'l']
        int2 = ['ul', 'lu', 'll']
        int3 = ['ull']

        if len(s) >= 3 and s[-3:] in int3:
            newS = s[:-3]
        elif len(s) >= 2 and s[-2:] in int2:
            newS = s[:-2]
        elif len(s) >= 1 and s[-1] in int1:
            newS = s[:-1]
        else:
            newS = s
        
        isN = False
        if len(newS) >= 1:
            if s[0] == '+':
                newS = newS[1:]
            elif s[0] == '-':
                newS = newS[1:]
                isN = True

        newS = newS.strip()
        if not newS.isalnum():
            return False, None
        try:
            if len(newS) >= 2 and newS[:2] == "0x":
                re = int(newS, 16)
            elif len(newS) >= 1 and newS[:1] == "0":
                re = int(newS, 8)
            else:
                re = int(newS)
            re = -re if isN else re
            return True, re
        except Exception:
            return False, None

    def __analysisValue(self, data):
        lengh = len(data)

        # 处理bool类型
        if data == "true":
            return True
        elif data == "false":
            return False

        # 处理字符
        if lengh >= 3 and data[0] == '\'' and data[-1] == '\'':
            try:
                re = ord(data[1:-1])
                return re
            except Exception:
                raise Exception("")

        # 处理字符串
        if lengh >= 2 and data[0] == '\"' and data[-1] == '\"':
            return data[1:-1]
        elif lengh >= 3 and data[:2] == "L\"" and data[-1] == '\"':
            return data[2:-1].decode("utf-8")

        # 处理聚集
        if lengh >= 3 and data[0] == '{' and data[-1] == '}':
            reList = []
            newData = data[1:-1]
            
            counter = 0
            restr = ""
            restrList = []
            for c in newData:
                if c == "," and counter == 0:
                    if restr != "":
                        restrList.append(restr)
                        restr = ""
                elif c == '{':
                    restr += c
                    counter += 1
                elif c == '}':
                    restr += c
                    counter -= 1
                else:
                    restr += c
            if restr != "":
                restrList.append(restr)

            for dl in restrList:
                dl = dl.strip()
                re = self.__analysisValue(dl)
                reList.append(re)
            return tuple(reList)
        
        # 处理整型 & 浮点型
        isI, re = self.isInt(data)
        if isI:
            return re
        
        isF, re = self.isFloat(data)
        if isF:
            return re

        return data

        # throw exception
        # raise Exception("error data, {}".format(data))

        
