# coding=utf-8

def index(filePath):
    print(filePath)
    return filePath

route_dict = {
    "index" : index
}

def route(func):
    name = func.__name__
    if route_dict.has_key(name):
        raise Exception("route name is exist!!!")
    else:
        route_dict[func.__name__] = func

print(route_dict.has_key("index"))