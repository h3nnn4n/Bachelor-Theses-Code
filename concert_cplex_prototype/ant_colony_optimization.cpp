#include "types.h"
#include "ant_colony_optimization.h"

_journey antColonyOptmization ( _csp *csp, _subproblem_info *sp, double *objValue ) {
    int    max_it        = 100;
    int    num_ants      =  10;
    double decay         = 0.1;
    double c_heur        = 2.5;
    double c_local_phero = 0.1;
    double c_greed       = 0.9;

    _graph graph;

    graph.resize(csp->N);

    for (int i = 0; i < (int) csp->graph.size(); ++i) {
        for (int j = 0; j < (int) csp->graph[i].size(); ++j) {
            _path p;
            p.dest = csp->graph[i][j].dest;
            graph[i].push_back(p);
        }
    }

    exit(0);

    _journey solution;
    return solution;
}
