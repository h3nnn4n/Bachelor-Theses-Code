#ifndef GREEDY_HEUR_H
#define GREEDY_HEUR_H

#include "types.h"

#include <ilcplex/ilocplex.h>
#include <ilcp/cpext.h>

_journey greedyHillClimbingHeur ( _csp *csp, _subproblem_info *sp, double *objValue ) ;
_journey greedyLpHeur ( _csp *csp, _subproblem_info *sp, IloArray<IloNumVarArray> y, IloEnv &env, IloCplex &cplex_final, double *objValue ) ;

#endif /* GREEDY_HEUR_H */
