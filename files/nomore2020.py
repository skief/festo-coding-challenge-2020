import re
import time

def check(a):
    regex = '2.*0.*2.*0'
    m = re.search(regex, a)
    return m is None


i = 0
found = 0
last = 0
s = time.time()
while found < 1000000:
    i += 1
    if check(str(i)):
        last = i
        found += 1

print(last)
print(time.time() - s)