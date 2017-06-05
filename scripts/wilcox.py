#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import numpy
import scypy
from scypy import stats

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

28: tabu total executions
29: tabu total time
30: tabu good executions
31: tabu good time
32: tabu bad executions
33: tabu bad time
34: tabu not unique executions
35: tabu not unique time

36: exact total executions
37: exact total time
38: exact good executions
39: exact good time
40: exact bad executions
41: exact bad time
42: exact not unique executions
43: exact not unique time

"""

data        = {}
avg_data    = {}
stddev_data = {}

if ( len(sys.argv) > 1 ):
    for name in sys.argv[1:]:
        avg    = [ 0.0 for i in range(0, 43) ]
        stddev = [ 0.0 for i in range(0, 43) ]
        lines  = [line.rstrip('\n') for line in open(name)]

        data[name] = []

        for line in lines:
            values = [float(x) for x in line.split(' ') if x]
            data[name].append(values)

        for line in lines:
            values = [float(x) for x in line.split(' ') if x]
            for k,v in enumerate(values):
                avg[k-1] += values[k-1] / len(lines)


        for line in lines:
            values = [float(x) for x in line.split(' ') if x]
            for k,v in enumerate(values):
                stddev[k-1] += ((avg[k-1] - values[k-1]) ** 2.0) / len(lines)

        avg_data[name] = avg
        stddev_data[name] = stddev

else:
    print("Not enough arguments")


