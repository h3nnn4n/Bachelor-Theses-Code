#ifndef MODEL_H
#define MODEL_H

#include "types.h"

#include <ilcplex/ilocplex.h>

void  populate_model (IloModel model, IloNumVarArray var, IloRangeArray con, _csp *t, std::vector<_journey> const &journeys, _csp *csp) ;

#endif /* MODEL_H */
