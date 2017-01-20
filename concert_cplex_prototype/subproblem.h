#ifndef SUBPROBLEM_H
#define SUBPROBLEM_H

#include "types.h"

#include <ilcplex/ilocplex.h>

_journey subproblem(IloNumArray reduced_costs, IloNumArray duals, _csp *t, std::vector<_journey> &journeys, double *reduced_cost) ;

#endif /* SUBPROBLEM_H */
