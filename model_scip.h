#ifndef MODEL_SCIP_H
#define MODEL_SCIP_H

#include "types.h"
#include "utils.h"

#include "scip/scip.h"
#include "scip/scipdefplugins.h"

SCIP_RETCODE buildModelScip (_csp &csp, _subproblem_info &subproblemInfo, SCIP** reducedMasterProblem, SCIP_VAR** vars, SCIP_CONS** cons) ;

#endif /* MODEL_SCIP_H */
