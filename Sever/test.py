# import testA
# import testB

# class a:
#     def __init__(self):
#         self.aa = 0

# bb= testA.b()
# cc = testB.c()

# aa = a()

# bb.insert(aa)
# cc.insert(aa)

# bb.mod()

# print(bb.a[0].aa)
# print(cc.a[0].aa)

a = [1,2,3,4]
c = []
for b in a:
    if b==2:
        c.append(b)
    if b==3:
        c.append(b)

for d in c:
    a.remove(d)
print(a)
