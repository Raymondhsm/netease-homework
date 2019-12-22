# coding=utf-8
class macros:
    def __init__(self, head, key, value):
        self.head = head
        self.key = key
        self.value = value


class PyMacroParser:

    HEADLIST = ["#else", "#endif", "#ifdef", "#ifndef", "#undef", "#define"]

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
            isInitializer, isDouble = False, False
            info = {
                "head": None,
                "key": None,
                "value": None
            }

            def storeData(_data):
                if info["head"] is None and _data in self.HEADLIST:
                    info["head"] = _data
                elif info["head"] is not None and info["key"] is None:
                    info["key"] = _data
                elif info["head"] is not None and info["key"] is not None and info["value"] is None:
                    info["value"] = self.__analysisValue(_data)
                else:
                    # throw exception
                    print("extra data")
                    pass

            for c in _line:
                if (c == " " or c == "\n") and not isAnnotation and not isDouble and not isInitializer:
                    if restr == "":
                        continue
                    else:
                        storeData(restr)
                        restr = ""

                elif c != " " and not isAnnotation and not isDouble and not isInitializer:
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
                        restr += c\
                
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
                else:
                    # throw exception
                    print("extra #")
                    pass

                preC = c

            if info["head"] is not None:
                self.loadList.append(macros(info["head"], info['key'], info['value']))
                print("{}, {}, {}".format(info["head"], info["key"], info["value"]))

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
        pass

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
                    print("extra #else")

            elif _macros.head == "#endif":
                if len(ifStack) > 0:
                    ifStack.pop()
                else:
                    # throw exception
                    print("extra #endif")

            else:
                # throw exception
                print("extra head:{}".format(_macros.head))

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

    def __analysisValue(self, data):
        lengh = len(data)

        # 处理bool类型
        if data == "true":
            return True
        elif data == "false":
            return False

        # 处理整型 & 浮点型
        try:
            isNegative = True if data[0] == '-' else False
            if isNegative:
                newData = data[1:]
            else:
                newData = data

            if len(newData) > 2 and newData[1] == '.':
                if newData[len(newData) - 1] == 'f':
                    re = float(newData[:-1])
                else:
                    re = float(newData)

            elif len(newData) > 3 and newData[:2] == "0x":
                re = int(newData, 16)
            else:
                re = int(newData)

            return -re if isNegative else re
        except Exception:
            pass

        # 处理字符
        if lengh >= 3 and data[0] == '\'' and data[-1] == '\'':
            try:
                re = ord(data[1:-1])
                return re
            except Exception:
                pass

        # 处理字符串
        if lengh > 2 and data[0] == '\"' and data[-1] == '\"':
            return data[1:-1]
        elif lengh > 3 and data[:2] == "L\"" and data[-1] == '\"':
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

        # throw exception
        print("error data, {}".format(data))

        
