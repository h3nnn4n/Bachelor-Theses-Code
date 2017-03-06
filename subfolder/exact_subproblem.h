#ifndef EXACT_SUBPROBLEM_H
#define EXACT_SUBPROBLEM_H

#include "types.h"

#include <ilcplex/ilocplex.h>
#include <ilcp/cpext.h>

_journey subproblemExactSolve(_csp *csp, _subproblem_info *sp, double *reduced_cost) ;

#endif /* EXACT_SUBPROBLEM_H */
