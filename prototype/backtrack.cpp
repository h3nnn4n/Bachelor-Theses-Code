#include "backtrack.h"
#include "types.h"

int backtrack(_csp csp, int pos, int cost, int lvl, int sol[], std::vector<_journey> &vec){
    if ( (int)csp.graph[pos].size() == 0 ) {
        _journey v;
        for (int i = 0; sol[i] != -1 ; ++i) {
            v.covered.push_back(sol[i]);
            v.cost = cost;
            vec.push_back(v);
        }
        return 1;
    }

    //if ( cost > csp.time_limit ) {
        //_journey v;
        //for (int i = 0; sol[i] != -1 ; ++i) {
            //v.covered.push_back(sol[i]);
            //v.cost = cost;
            //vec.push_back(v);
        //}
        //return 0;
    //}

    int w = 0;

    for (int i = 0; i < (int)csp.graph[pos].size(); ++i) {

        if ( cost + csp.graph[pos][i].cost < csp.time_limit ) {
            sol[lvl    ] = csp.graph[pos][i].dest;
            sol[lvl + 1] = -1;
            w += backtrack(csp, csp.graph[pos][i].dest, csp.graph[pos][i].cost + cost, lvl + 1, sol, vec);
        } else {
            _journey v;
            for (int i = 0; sol[i] != -1 ; ++i) {
                v.covered.push_back(sol[i]);
                v.cost = cost;
                vec.push_back(v);
            }
        }
    }

    return w;
}
