# coding=utf-8
import PyMacroParser

# pp = PyMacroParser.PyMacroParser()
# pp.load("C:/Users/Administrator/Desktop/wy/python/abc.cpp")
# pp.dumpDict()

# head = ["ele", "sadas"]
# a = 'ele'
# if a in head:
#     print("s")

a1 = PyMacroParser.PyMacroParser()
# a2 = PyMacroParser.PyMacroParser()
a1.load("C:/Users/Administrator/Desktop/wy/python/abc.cpp")
# filename = "b.cpp"
# a1.dump(filename)
# a2.load(filename)
# a2.dumpDict()
# a1.preDefine("MC1;MC2")
a1.dumpDict()
# a1.dump("c.cpp")
