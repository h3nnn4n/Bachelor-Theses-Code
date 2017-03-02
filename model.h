#ifndef MODEL_H
#define MODEL_H

#include "types.h"
#include "utils.h"

#include "scip/scip.h"
#include "scip/scipdefplugins.h"

//SCIP_RETCODE buildModel (SCIP* reducedMasterProblem, SCIP_VAR** vars, SCIP_CONS** cons) ;
SCIP_RETCODE buildModel (_csp &csp, _subproblem_info &subproblemInfo, SCIP** reducedMasterProblem, SCIP_VAR** vars, SCIP_CONS** cons) ;

#endif /* MODEL_H */
