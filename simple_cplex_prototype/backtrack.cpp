#include <cstdio>
#include "backtrack.h"
#include "types.h"

int backtrack(_csp csp, int pos, int cost, int time, int lvl, int sol[], std::vector<_journey> &vec){
    //return backtrack_first_feasible   ( csp, pos, cost, time, lvl, sol, vec);
    //return backtrack_biggest_feasible ( csp, pos, cost, time, lvl, sol, vec);
    return backtrack_all_feasible     ( csp, pos, cost, time, lvl, sol, vec);
}

int backtrack_all_feasible(_csp csp, int pos, int cost, int time, int lvl, int sol[], std::vector<_journey> &vec){
    if ( lvl == 0 ) {
        sol[lvl    ] = pos;
        sol[lvl + 1] = -1;

        _journey v;
        for (int j = 0; sol[j] != -1 ; ++j) {
            v.covered.push_back(sol[j]);
            v.cost = cost;
            v.time = time + csp.task[pos].end_time - csp.task[pos].start_time;
            time += csp.task[pos].end_time - csp.task[pos].start_time;
        }
        lvl += 1;
    }

    for (int i = 0; i < (int)csp.graph[pos].size(); ++i) {
        if ( time + csp.task[i].end_time - csp.task[i].start_time <= csp.time_limit ) {
            sol[lvl    ] = csp.graph[pos][i].dest;
            sol[lvl + 1] = -1;
            _journey v;
            v.cost = cost + csp.graph[pos][i].cost;
            v.time = 0;
            for (int j = 0; sol[j] != -1 ; ++j) {
                v.covered.push_back(sol[j]);
                v.time += csp.task[sol[j]-1].end_time - csp.task[sol[j]-1].start_time;
            }
            vec.push_back(v);

            //printf("time:  ");
            //for (int j = 0; sol[j] != -1 ; ++j) {
                //printf("%3d ", csp.task[sol[j]-1].end_time - csp.task[sol[j]-1].start_time);
            //}

            //printf("\nNodes: ");
            //for (int j = 0; sol[j] != -1 ; ++j) {
                //printf("%3d ", sol[j]);
            //}
            //printf("\n");
            //printf("Cost: %6d   Time: %6d\n", v.cost, v.time);
            //printf("\n\n");

            backtrack(csp, csp.graph[pos][i].dest, cost + csp.graph[pos][i].cost, csp.task[i].end_time - csp.task[i].start_time + time, lvl + 1, sol, vec);
        } else {

        }
    }

    return 0;
}

int backtrack_biggest_feasible(_csp csp, int pos, int cost, int time, int lvl, int sol[], std::vector<_journey> &vec){
    int last = 0;
    if ( lvl == 0 ) {
        sol[lvl    ] = pos;
        sol[lvl + 1] = -1;
        lvl += 1;
    }

    for (int i = 0; i < (int)csp.graph[pos].size(); ++i) {
        if ( cost + csp.task[i].end_time - csp.task[i].start_time < csp.time_limit ) {
            last = 1;
            sol[lvl    ] = csp.graph[pos][i].dest;
            sol[lvl + 1] = -1;
            backtrack(csp, csp.graph[pos][i].dest, csp.task[i].end_time - csp.task[i].start_time + cost, time, lvl + 1, sol, vec);
        } else {

        }
    }

    if ( last == 0 && cost < csp.time_limit && cost > 0) {
        _journey v;
        for (int j = 0; sol[j] != -1 ; ++j) {
            v.covered.push_back(sol[j]);
            v.cost = cost;
        }
        vec.push_back(v);
    }

    return last;
}

int backtrack_first_feasible(_csp csp, int pos, int cost, int time, int lvl, int sol[], std::vector<_journey> &vec) {
    int last = 0;
    if ( lvl == 0 ) {
        sol[lvl    ] = pos;
        sol[lvl + 1] = -1;

        //_journey v;
        //for (int j = 0; sol[j] != -1 ; ++j) {
            //v.covered.push_back(sol[j]);
            //v.cost = cost;
            //v.cost = cost + csp.graph[pos][i].cost;
        //}
        //vec.push_back(v);
        lvl += 1;
    }

    if ( last == 0 && cost < csp.time_limit && cost > 0 && lvl == 2) {
        //printf("Adding: ");
        _journey v;
        for (int j = 0; sol[j] != -1 ; ++j) {
            v.covered.push_back(sol[j]);
            v.cost = cost;
            //printf("%d ", sol[j]);
        }
        //printf("\n");
        vec.push_back(v);
        return 0;
    }

    for (int i = 0; i < (int)csp.graph[pos].size(); ++i) {
        if ( cost + csp.task[i].end_time - csp.task[i].start_time < csp.time_limit ) {
            last = 1;
            sol[lvl    ] = csp.graph[pos][i].dest;
            sol[lvl + 1] = -1;
            //_journey v;
            //if ( f ) {
            //for (int j = 0; sol[j] != -1 ; ++j) {
                //v.covered.push_back(sol[j]);
                //v.cost = cost + csp.graph[pos][i].cost;
                //v.cost = cost + csp.task[i].end_time - csp.task[i].start_time;
            //}
            //}
            //vec.push_back(v);
            backtrack(csp, csp.graph[pos][i].dest, csp.task[i].end_time - csp.task[i].start_time + cost, time, lvl + 1, sol, vec);
        } else {

        }
    }

    return last;
}
