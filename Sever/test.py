import json

str = '{"com":123,"data":{"pos":"asd","rotate":"asd"}}'
di = {"sad":123, "asdasd":"12312","asdas":{"asd":123}}
j=json.dumps(di)
print(j)