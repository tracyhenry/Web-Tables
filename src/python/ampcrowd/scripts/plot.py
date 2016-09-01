#!/usr/bin/env python
prefix = 'go_'

file_name = '../ampcrowd/answers.txt'
f = open(file_name, 'r')

m1 = {}
m2 = {}
m3 = {}

for line in f.readlines():
    line = line[len(prefix):]
    tab_pos = line.find('\t')
    s1 = line[0:tab_pos]
    s2 = line[tab_pos + 1:]
    enrichment_type = int(s1[0:s1.find('_')])
    rank = int(s1[s1.find('_') + 1:])
    score = float(s2)
    if enrichment_type == 1:
        m1[rank] = score
    elif enrichment_type == 2:
        m2[rank] = score
    else:
        m3[rank] = score
f.close()

maxk1 = 1000
maxk2 = 1000
maxk3 = 1000

f = open('../../../../data/Result/enrichment/plot1.txt', 'w')
for i in range(1, maxk1):
    sum = 0
    tot = 0
    for j in range(1, i):
        if j in m1.keys():
            sum += m1[j]
            tot += 1
    if tot > 0:
        sum /= float(tot)
    f.write(str(sum) + '\n')
f.close()

f = open('../../../../data/Result/enrichment/plot2.txt', 'w')
for i in range(1, maxk2):
    sum = 0
    tot = 0
    for j in range(1, i):
        if j in m2.keys():
            sum += m2[j]
            tot += 1
    if tot > 0:
        sum /= float(tot)
    f.write(str(sum) + '\n')
f.close()

f = open('../../../../data/Result/enrichment/plot3.txt', 'w')
for i in range(1, maxk3):
    sum = 0
    tot = 0
    for j in range(1, i):
        if j in m3.keys():
            sum += m3[j]
            tot += 1
    if tot > 0:
        sum /= float(tot)
    f.write(str(sum) + '\n')
f.close()

