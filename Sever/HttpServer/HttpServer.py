# coding=utf-8
from SimpleHTTPServer import SimpleHTTPRequestHandler
import BaseHTTPServer
import os,io,shutil
import cgi
import Route

class MySimpleHTTPRequestHandler(SimpleHTTPRequestHandler):

    def set_headers(self,status):
        self.send_response(status)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def send_datas(self,contents):
        enc = "UTF-8"
        #contents = contents.encode(enc)     
        StrContents = str(contents)     
        f = io.BytesIO()
        f.write(StrContents)
        f.seek(0)  
        self.send_response(200)  
        self.send_header("Content-type", "text/html; charset=%s" % enc)  
        self.send_header("Content-Length", str(len(StrContents)))  
        self.end_headers() 
        shutil.copyfileobj(f,self.wfile)

    def do_GET(self):
        status, result = self.__ParseGetInfo(str(self.path))
        if status == 200:
            self.send_datas(result)
        else:
            self.set_headers(status)

    def do_POST(self):
        datasets = cgi.FieldStorage(fp = self.rfile,headers = self.headers,environ = {'REQUEST_METHOD': 'POST'})
        id = datasets.getvalue('id')
        name = datasets.getvalue('name')
        
        msg = "name=="+str(name)+"   id=="+str(id)
        flag = 1
        results = {'status':flag,'msg':msg}
        self.send_datas(str(results))

    def __ParseGetInfo(self, value):
        value = value[1:]
        values = value.split("?")

        if len(values) > 2 or len(values) == 0:
            return 400, None
        else:
            if not Route.route_dict.has_key(values[0]):
                return 400, None
            elif len(values) == 1:
               return 200, Route.route_dict[values[0]]()
            else:
                params = values[1].split("&")
                paramDict = {}
                for param in params:
                    keyValue = param.split("=")
                    if len(keyValue)<=0 or len(keyValue)>2:
                        return 400, None
                    else:
                        paramDict[keyValue[0]] = keyValue[1]
                return 200, Route.route_dict[values[0]](paramDict)

def start_server():
    server_host = '127.0.0.1'
    server_port = 8080
    httpd = BaseHTTPServer.HTTPServer((server_host,server_port), MySimpleHTTPRequestHandler)
    print('exe_server started on '+str(server_host)+' server_port:'+str(server_port))
    httpd.serve_forever()

if __name__ == "__main__":
    start_server()
