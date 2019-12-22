# coding=utf-8
class macros:
    def __init__(self, head, key, value):
        self.head = head
        self.key = key
        self.value = value


class PyMacroParser:

    HEADLIST = ["#else", "#endif", "#ifdef", "#ifndef", "#undef", "#define"]

    def __init__(self):
        self.preDefineList = []
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
                    info["value"] = _data
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
        self.preDefineList = []

        if s == "":
            return
        else:
            slist = s.split(',')
            for sl in slist:
                self.preDefineList.append(sl)

    def dumpDict(self):
        print(self.__analysisMacros())

    def dump(self, f):
        pass

    def __analysisMacros(self):
        macrosDict = {}
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
                    macrosDict.pop(_macros.key)
                elif _macros.key in self.preDefineList:
                    self.preDefineList.pop(_macros.key)

            elif _macros.head == "#ifdef" or _macros.head == "#ifndef":
                condition = _macros.key in macrosDict or _macros.key in self.preDefineList
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
