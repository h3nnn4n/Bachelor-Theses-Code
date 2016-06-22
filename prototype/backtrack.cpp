#include "backtrack.h"
#include "types.h"

int backtrack(_csp csp, int pos, int cost){
    if ( (int)csp.graph[pos].size() == 0 )
        return 1;

    if ( cost > csp.time_limit )
        return 0;

    int w = 0;

    for (int i = 0; i < (int)csp.graph[pos].size(); ++i) {
        w += backtrack(csp, csp.graph[pos][i].dest, csp.graph[pos][i].cost + cost);
    }

    return w;
}
