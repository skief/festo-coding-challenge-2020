import pandas as pd
from collections import namedtuple
import itertools

Path = namedtuple('Path', 'f t')

rs = ['r%d' % i for i in range(1, 6)]
cs = ['c%d' % i for i in range(1, 6)]

names = ['w', *rs, *cs, 'h']

csv = pd.read_csv('3_2_delivery_service.csv', names=names)
costs = {}

for x in range(len(names)):
    for y in range(len(names)):
        f = names[y]
        t = names[x]

        val = csv[names[x]][y]
        if '-' in val:
            continue
        else:
            costs[Path(f, t)] = int(val)


def cost(order):
    order = ['w'] + list(order) + ['h']
    c = 0
    for i in range(len(order) - 1):
        c += costs.get(Path(order[i], order[i+1]), 9999)

    return c

def allowed(p):
    global rs
    global cs

    loaded = []
    for i in range(len(p)):
        if p[i] in rs:
            loaded.append(p[i])
            if len(loaded) > 3:
                return False
        else:
            try:
                loaded.remove('r' + p[i][1])
            except:
                return False
    return True


shortestC = 9999999
shortest = []

n = [*rs, *cs]
for perm in itertools.permutations(n):
    if not allowed(perm):
        continue

    c = cost(perm)
    if c < shortestC:
        shortestC = c
        shortest = perm
print(shortestC)
print(''.join(shortest))
