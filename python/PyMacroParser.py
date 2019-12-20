class PyMacroParser:
    

    def load(self, f):
        pass

    def preDefine(self, s):
        pass

    def dumpDict(self):
        pass

    def dump(self,f):
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