#ifndef MODEL_H
#define MODEL_H

#include "types.h"

#include <ilcplex/ilocplex.h>

void  buildModelCplex (IloModel model, IloNumVarArray var, IloRangeArray con, IloObjective obj, std::vector<_journey> const &journeys, _csp *csp, _subproblem_info *subproblemInfo) ;

#endif /* MODEL_H */
