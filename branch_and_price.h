#ifndef BRANCH_AND_PRICE_H
#define BRANCH_AND_PRICE_H

#include "scip/scip.h"
#include "scip/scipdefplugins.h"

#include "pricer_csp.h"

#pragma GCC diagnostic ignored "-Wunused-variable"
static const char* CSP_PRICER_NAME = "CSP_Pricer";

SCIP_RETCODE runSPP (_csp &csp, _subproblem_info &subproblemInfo) ;

#endif /* BRANCH_AND_PRICE_H */
