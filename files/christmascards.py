import pandas as pd
from collections import namedtuple
import itertools

Path = namedtuple('Path', 'f t')

names = [c for c in 'wabcdefgho']
n = [c for c in 'abcdefgh']

csv = pd.read_csv('1_2_christmas_cards.csv', names=names)

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
    order = ['w'] + list(order) + ['o']
    c = 0
    for i in range(len(order) - 1):
        c += costs.get(Path(order[i], order[i+1]), 9999)

    return c


shortestC = 9999999
shortest = []
for perm in itertools.permutations(n):
    c = cost(perm)
    if c < shortestC:
        shortestC = c
        shortest = perm

print(shortestC)
print(''.join(shortest))
