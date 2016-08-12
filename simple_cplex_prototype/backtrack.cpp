#include <cstdio>
#include "backtrack.h"
#include "types.h"

int backtrack(_csp csp, int pos, int cost, int lvl, int sol[], std::vector<_journey> &vec){
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

    for (int i = 0; i < (int)csp.graph[pos].size(); ++i) {
        if ( cost + csp.graph[pos][i].cost < csp.time_limit ) {
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
            backtrack(csp, csp.graph[pos][i].dest, csp.task[i].end_time - csp.task[i].start_time + cost, lvl + 1, sol, vec);
        } else {

        }
    }

    if ( last == 0 && cost < csp.time_limit && cost > 0) {
        //printf("Adding: ");
        _journey v;
        for (int j = 0; sol[j] != -1 ; ++j) {
            v.covered.push_back(sol[j]);
            v.cost = cost;
            //printf("%d ", sol[j]);
        }
        //printf("\n");
        vec.push_back(v);
    }

    return last;
}