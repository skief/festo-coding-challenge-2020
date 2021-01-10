import pandas as pd
from collections import namedtuple
import itertools

Path = namedtuple('Path', 'f t')

ss = ['s1', 's2']
ps = ['p1', 'p2']
hs = ['h1', 'h2']
ds = ['d1', 'd2']
ts = ['t1', 't2']
names = ['w', *ss, *ps, *hs, *ds, *ts, 'h']

csv = pd.read_csv('2_2_christmas_shopping.csv', names=names)
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


shortestC = 9999999
shortest = []
for s in ss:
    for p in ps:
        for h in hs:
            for d in ds:
                for t in ts:
                    possible = [s, p, h, d, t]

                    for perm in itertools.permutations(possible):
                        c = cost(perm)
                        if c < shortestC:
                            shortestC = c
                            shortest = perm

print(shortestC)
print(''.join(shortest))
