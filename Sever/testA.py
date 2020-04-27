a = {
    1:1,
    2:2,
    3:3,
    4:4,
    5:5
}

for b in a:
    if b == 3:
        del a[b]

print(a)