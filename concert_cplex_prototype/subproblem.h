#ifndef SUBPROBLEM_H
#define SUBPROBLEM_H

#include "types.h"

#include <ilcplex/ilocplex.h>

void subproblem(IloNumArray reduced_costs, IloNumArray duals, _csp *csp, std::vector<_journey> &journeys) ;

#endif /* SUBPROBLEM_H */
