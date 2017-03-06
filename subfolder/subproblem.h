#ifndef SUBPROBLEM_H
#define SUBPROBLEM_H

#include "types.h"

#include <vector>
#include <map>

void init_subproblem_info ( _subproblem_info *sp, _csp *csp ) ;

_journey subproblem(_csp *csp, _subproblem_info *sp, double *reduced_cost) ;

#endif /* SUBPROBLEM_H */
