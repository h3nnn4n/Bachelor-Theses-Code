#!/usr/bin/env python
# -*- coding: utf-8 -*-

from subprocess import call

data = [('csp50' , 27 ), ('csp50' , 28) , ('csp50' , 29 ),
        ('csp100', 42 ), ('csp100', 43) , ('csp100', 44 ),
        ('csp150', 67 ), ('csp150', 68) , ('csp150', 69 ),
        ('csp200', 86 ), ('csp200', 87) , ('csp200', 88 ),
        ('csp250', 111), ('csp250', 112), ('csp250', 112),
        ('csp300', 131), ('csp300', 132), ('csp300', 130)]

#data = [('csp50' , 27 ), ('csp50' , 28) , ('csp50' , 29 )]
#data = [('csp100', 42 ), ('csp100', 43) , ('csp100', 44 )]
#data = [('csp150', 67 ), ('csp150', 68) , ('csp150', 69 )]
#data = [('csp200', 86 ), ('csp200', 87) , ('csp200', 88 )]
#data = [('csp250', 111), ('csp250', 112), ('csp250', 112)]
#data = [('csp300', 130), ('csp300', 131), ('csp300', 132)]

#data = [('csp100', 42 )]

for i in data:
    for j in range(40):
        print(str(i) + " " + str(j))
        call(["./magic", "/home/h3nnn4n/csp_prototypes/instances/beasley/" + i[0], str(i[1])])
