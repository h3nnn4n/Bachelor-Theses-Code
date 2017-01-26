#ifndef SUBPROBLEM_H
#define SUBPROBLEM_H

#include "types.h"

#include <vector>
#include <map>

#include <ilcplex/ilocplex.h>

_journey subproblem(IloNumArray reduced_costs, IloNumArray duals, _csp *t, std::vector<_journey> &journeys, double *reduced_cost, std::map<std::vector<int>, bool> &usedJourneys) ;

#endif /* SUBPROBLEM_H */
