# coding=utf-8
class macros:
    def __init__(self, head, key, value):
        self.head = head
        self.key = key
        self.value = value


HEADLIST = ["#else", "#endif", "#ifdef", "#ifndef", "#undef", "#define"]


class PyMacroParser:

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
            isDouble = False
            info = {
                "head": None,
                "key": None,
                "value": None
            }

            def storeData(_data):
                if info["head"] is None and _data in HEADLIST:
                    info["head"] = _data
                elif info["head"] is not None and info["key"] is None:
                    info["key"] = _data
                elif info["head"] is not None and info["key"] is not None and info["value"] is None:
                    info["value"] = _data
                else:
                    # throw exception
                    pass

            for c in _line:
                if (c == " " or c == "\n") and not isAnnotation and not isDouble:
                    if restr == "":
                        continue
                    else:
                        storeData(restr)
                        restr = ""

                elif c != " " and not isAnnotation and not isDouble:
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
                else:
                    # throw exception
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
        pass

    def dump(self, f):
        pass

    def __fileToLineList(self, fileName):
        lineList = []

        f = open(fileName)
        line = f.readline()
        while line:
            lineList.append(line)
            line = f.readline()
        f.close()

        return lineList

    def __splitLine(self, _line):
        _lineBlock = _line.split(' ')

        # 剔除空块
        _lineBlock = [i for i in _lineBlock if(len(str(i)) != 0)]

        if len(_lineBlock) == 0:
            return None, None, None

        head = [
            {"#else", "#endif"},
            {"#ifdef", "#ifndef", "#undef"},
            {"#define"}
        ]

        _block = _lineBlock[0]
        if _block in head[0]:
            return _block, None, None

        elif _block in head[1]:
            if len(_lineBlock) == 2:
                return _block, _lineBlock[1], None
            else:
                # throw exception
                pass

        elif _block in head[2]:
            if len(_lineBlock) == 2:
                return _block, _lineBlock[1], None
            elif len(_lineBlock) == 3:
                return _block, _lineBlock[1], _lineBlock[2]
            else:
                # throw exception
                pass

        else:
            # throw exception
            pass

    def __removeAnnotation(self, _lineList):
        for _line in _lineList:
            if "//" not in _line and "/*" not in _line:
                continue

