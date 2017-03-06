#!/usr/bin/env python
# -*- coding: utf-8 -*-

f = open("csp50.txt")

r = f.readline().split(' ')
n = r[1].rstrip('\n')
time = r[2].rstrip('\n')

print(n, time)

for k, i in enumerate(f.readlines()):
    if k < 50:
        r = i.rstrip('\n').lstrip(' ').split(' ')
        a, b = int(r[0]), int(r[1])
        print("{0:3d} {1:5d} {2:5d} {3:5d}".format(k, a, b, b-a))
