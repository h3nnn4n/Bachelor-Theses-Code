#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from subprocess import call

ntests = 10

magic = "/home/h3nnn4n/csp_prototypes/"

if ( len(sys.argv) == 1 ):
    magic += "magic"
else:
    magic += sys.argv[1]

data  = []
data += [('csp50' , 27 ), ('csp50' , 28) , ('csp50' , 29 )]
data += [('csp100', 42 ), ('csp100', 43) , ('csp100', 44 )]
data += [('csp150', 67 ), ('csp150', 68) , ('csp150', 69 )]
data += [('csp200', 86 ), ('csp200', 87) , ('csp200', 88 )]
data += [('csp250', 111), ('csp250', 112), ('csp250', 113)]
#data += [('csp300', 130), ('csp300', 131), ('csp300', 132)]

cflags  = []
cflags += ['run_chvatal']
cflags += ['run_unary']
cflags += ['run_random']
cflags += ['run_chvatal run_unary']
cflags += ['run_random run_unary']
cflags += ['run_random run_chvatal']
cflags += ['run_random run_chvatal run_unary']

for i in data:
    for j in range(ntests):
        for k in cflags:
            call(["make", "clean"])
            ps = []
            for n in k.split():
                ps.append('-D' + n)
            p = 'CFLAGS="' + ' '.join(ps) + '"'
            call(["make", "-j", p])
            print(magic + ' ' + "/home/h3nnn4n/csp_prototypes/instances/beasley/" + i[0] + ' ' + str(i[1]) + ' ' + ("%s_%d_%s.log" % (i[0], i[1], k.replace(' ', '-'))))
            call([magic, "/home/h3nnn4n/csp_prototypes/instances/beasley/" + i[0], str(i[1]), ("%s_%d_%s.log" % (i[0], i[1], k.replace(' ', '-')))])
