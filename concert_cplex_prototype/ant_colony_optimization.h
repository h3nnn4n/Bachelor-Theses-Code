#ifndef ANT_COLONY_OPTIMIZATION_H
#define ANT_COLONY_OPTIMIZATION_H

#include "types.h"

typedef struct {
    int dest;
    double pheromone;
    double time;
    double cost;
    double dual_cost;
    double scaled_cost;
} _path;

typedef std::vector<std::vector<_path> > _graph;

_journey antColonyOptmization ( _csp *csp, _subproblem_info *sp, double *objValue ) ;

#endif /* ANT_COLONY_OPTIMIZATION_H */
