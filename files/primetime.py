
def check(x):
    while x != 1:
        if x % 7 == 0:
            x = x // 7
        elif x % 11 == 0:
            x = x // 11
        elif x % 13 == 0:
            x = x // 13
        else:
            return False

    return True


found = 0
i = 0
while found < 200:
    i += 1

    if check(i):
        found += 1
        print(found, i)

print(i)