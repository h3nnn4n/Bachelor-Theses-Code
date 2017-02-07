#ifndef SUBPROBLEM_H
#define SUBPROBLEM_H

#include "types.h"

#include <vector>
#include <map>

#include <ilcplex/ilocplex.h>

void init_subproblem_info ( _subproblem_info *sp, _csp *csp ) ;
void update_subproblem_duals ( _subproblem_info *sp, _csp *csp, IloNumArray duals ) ;

_journey subproblem(IloNumArray duals, _csp *csp, _subproblem_info *subproblemInfo, double *reduced_cost) ;

#endif /* SUBPROBLEM_H */
