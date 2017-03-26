#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import numpy

"""
1: init time
2: total time
3: cplex time
4: scip time

5: hillClimbing total executions
6: hillClimbing total time
7: hillClimbing good executions
8: hillClimbing good time
9: hillClimbing bad executions
10: hillClimbing bad time
11: hillClimbing not unique executions
12: hillClimbing not unique time

13: sa total executions
14: sa total time
15: sa good executions
16: sa good time
17: sa bad executions
18: sa bad time
19: sa not unique executions
20: sa not unique time

21: aco total executions
22: aco total time
23: aco good executions
24: aco good time
25: aco bad executions
26: aco bad time
27: aco not unique executions
28: aco not unique time

29: tabu total executions
30: tabu total time
31: tabu good executions
32: tabu good time
33: tabu bad executions
34: tabu bad time
35: tabu not unique executions
36: tabu not unique time

37: exact total executions
38: exact total time
39: exact good executions
40: exact good time
41: exact bad executions
42: exact bad time
43: exact not unique executions
44: exact not unique time

"""

total = 0
hit = 0
miss = 0

if ( len(sys.argv) > 1 ):
    for name in sys.argv[1:]:
        avg    = [ 0.0 for i in range(0,43) ]
        stddev = [ 0.0 for i in range(0,43) ]
        lines  = [line.rstrip('\n') for line in open(name)]

        for line in lines:
            values = [float(x) for x in line.split(' ') if x]
            for k,v in enumerate(values):
                avg[k-1] += values[k-1] / len(lines)


        for line in lines:
            values = [float(x) for x in line.split(' ') if x]
            for k,v in enumerate(values):
                stddev[k-1] += ((avg[k-1] - values[k-1]) ** 2.0) / len(lines)

        #print(name, end=' ')

        #for i in avg:
            #print("%8.4f" % i, end=' ')

        # SA
        # total += avg[12]
        # hit   += avg[14]
        # miss  += avg[16]
        # miss  += avg[18]

        ## HC
        total += avg[ 4]
        hit   += avg[ 6]
        miss  += avg[ 8]
        miss  += avg[10]

        # # ACO
        #total += avg[20]
        #hit   += avg[22]
        #miss  += avg[24]
        #miss  += avg[26]

        # tabu
        #total += avg[28]
        #hit   += avg[30]
        #miss  += avg[32]
        #miss  += avg[34]

    #print()

    #for i in stddev:
        #print("%8.4f" % i, end=' ')

    #print()
    print(total, hit, miss)
else:
    print("Not enough arguments")

