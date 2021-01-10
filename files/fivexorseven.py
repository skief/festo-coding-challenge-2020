last = 0
i = 0
count = 0
while count < 1000:
    i += 1
    c5 = '5' in str(i)
    c7 = '7' in str(i)

    if c5 ^ c7:
        last = i
        count += 1

print(last)