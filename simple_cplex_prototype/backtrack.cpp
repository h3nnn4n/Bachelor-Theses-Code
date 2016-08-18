#include <cstdio>
#include "backtrack.h"
#include "types.h"

int backtrack(_csp csp, int pos, int cost, int time, int lvl, int sol[], std::vector<_journey> &vec){
    //return backtrack_biggest_feasible ( csp, pos, cost, time, lvl, sol, vec);
    //return backtrack_first_feasible   ( csp, pos, cost, time, lvl, sol, vec);
    return backtrack_all_feasible     ( csp, pos, cost, time, lvl, sol, vec);
}

int backtrack_biggest_feasible(_csp csp, int pos, int cost, int time, int lvl, int sol[], std::vector<_journey> &vec){
    int last, t2;
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

    last = 1;
    for (int i = 0; i < (int)csp.graph[pos].size(); ++i) {
        t2 = 0;
        for (int j = 0; sol[j] != -1 ; ++j)
            t2 += csp.task[sol[j]-1].end_time - csp.task[sol[j]-1].start_time;

        if ( 1 ) {
        //if ( t2 + csp.task[csp.graph[pos][i].dest-1].end_time - csp.task[csp.graph[pos][i].dest-1].start_time <= csp.time_limit ) {
            last = 0;
            sol[lvl    ] = csp.graph[pos][i].dest;
            sol[lvl + 1] = -1;
            _journey v;
            v.cost = 0;
            v.time = 0;
            for (int j = 0; sol[j] != -1 ; ++j) {
                v.covered.push_back(sol[j]);
                v.time += csp.task[sol[j]-1].end_time - csp.task[sol[j]-1].start_time;
            }

            for (int j = 0; sol[j+1] != -1 ; ++j) {
                for (int k = 0; k < (int) csp.graph[sol[j]].size(); ++k) {
                    if ( csp.graph[sol[j]][k].dest == sol[j+1] ) {
                        v.cost += csp.graph[sol[j]][k].cost;
                    }
                }
            }

            if ( backtrack(csp, csp.graph[pos][i].dest, 0, 0, lvl + 1, sol, vec) ) {
                vec.push_back(v);
            }
        } else {

        }
    }

    return last;
}

int backtrack_all_feasible(_csp csp, int pos, int cost, int time, int lvl, int sol[], std::vector<_journey> &vec){
    int last, t2;
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

    last = 1;
    for (int i = 0; i < (int)csp.graph[pos].size(); ++i) {
        t2 = 0;
        for (int j = 0; sol[j] != -1 ; ++j)
            t2 += csp.task[sol[j]-1].end_time - csp.task[sol[j]-1].start_time;

        if ( t2 + csp.task[csp.graph[pos][i].dest-1].end_time - csp.task[csp.graph[pos][i].dest-1].start_time <= csp.time_limit || 1) {
            last = 0;
            sol[lvl    ] = csp.graph[pos][i].dest;
            sol[lvl + 1] = -1;
            _journey v;
            v.cost = 0;
            v.time = 0;
            for (int j = 0; sol[j] != -1 ; ++j) {
                v.covered.push_back(sol[j]);
                v.time += csp.task[sol[j]-1].end_time - csp.task[sol[j]-1].start_time;
            }

            for (int j = 0; sol[j+1] != -1 ; ++j) {
                for (int k = 0; k < (int) csp.graph[sol[j]].size(); ++k) {
                    if ( csp.graph[sol[j]][k].dest == sol[j+1] ) {
                        v.cost += csp.graph[sol[j]][k].cost;
                    }
                }
            }

            printf("%6d ", v.time);
            if ( v.time <= csp.time_limit ) {
                vec.push_back(v);
                printf(" ok\n");
            } else {
                printf(" rip\n");
            }

            backtrack(csp, csp.graph[pos][i].dest, 0, 0, lvl + 1, sol, vec);
        } else {

        }
    }

    return last;
}

//int backtrack_biggest_feasible(_csp csp, int pos, int cost, int time, int lvl, int sol[], std::vector<_journey> &vec){
    //int last = 0;
    //if ( lvl == 0 ) {
        //sol[lvl    ] = pos;
        //sol[lvl + 1] = -1;
        //lvl += 1;
    //}

    //for (int i = 0; i < (int)csp.graph[pos].size(); ++i) {
        //if ( cost + csp.task[i].end_time - csp.task[i].start_time < csp.time_limit ) {
            //last = 1;
            //sol[lvl    ] = csp.graph[pos][i].dest;
            //sol[lvl + 1] = -1;
            //backtrack(csp, csp.graph[pos][i].dest, csp.task[i].end_time - csp.task[i].start_time + cost, time, lvl + 1, sol, vec);
        //} else {

        //}
    //}

    //if ( last == 0 && cost < csp.time_limit && cost > 0) {
        //_journey v;
        //for (int j = 0; sol[j] != -1 ; ++j) {
            //v.covered.push_back(sol[j]);
            //v.cost = cost;
        //}
        //vec.push_back(v);
    //}

    //return last;
//}

//int backtrack_first_feasible(_csp csp, int pos, int cost, int time, int lvl, int sol[], std::vector<_journey> &vec) {
    //int last = 0;
    //if ( lvl == 0 ) {
        //sol[lvl    ] = pos;
        //sol[lvl + 1] = -1;

        ////_journey v;
        ////for (int j = 0; sol[j] != -1 ; ++j) {
            ////v.covered.push_back(sol[j]);
            ////v.cost = cost;
            ////v.cost = cost + csp.graph[pos][i].cost;
        ////}
        ////vec.push_back(v);
        //lvl += 1;
    //}

    //if ( last == 0 && cost < csp.time_limit && cost > 0 && lvl == 2) {
        ////printf("Adding: ");
        //_journey v;
        //for (int j = 0; sol[j] != -1 ; ++j) {
            //v.covered.push_back(sol[j]);
            //v.cost = cost;
            ////printf("%d ", sol[j]);
        //}
        ////printf("\n");
        //vec.push_back(v);
        //return 0;
    //}

    //for (int i = 0; i < (int)csp.graph[pos].size(); ++i) {
        //if ( cost + csp.task[i].end_time - csp.task[i].start_time < csp.time_limit ) {
            //last = 1;
            //sol[lvl    ] = csp.graph[pos][i].dest;
            //sol[lvl + 1] = -1;
            ////_journey v;
            ////if ( f ) {
            ////for (int j = 0; sol[j] != -1 ; ++j) {
                ////v.covered.push_back(sol[j]);
                ////v.cost = cost + csp.graph[pos][i].cost;
                ////v.cost = cost + csp.task[i].end_time - csp.task[i].start_time;
            ////}
            ////}
            ////vec.push_back(v);
            //backtrack(csp, csp.graph[pos][i].dest, csp.task[i].end_time - csp.task[i].start_time + cost, time, lvl + 1, sol, vec);
        //} else {

        //}
    //}

    //return last;
//}
