# RC_5_5 功能: 找零錢。
count = 0
for x in range(6):
    for y in range(11):
        for z in range(51):
            if 10*x+5*y+z == 50:
                count = count + 1
print('%s %d %s' %('共有', count, '種換法'))