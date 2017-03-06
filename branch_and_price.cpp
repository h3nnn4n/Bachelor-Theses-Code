#include <iostream>
#include <vector>

#include <time.h>

#include "random.h"
#include "reader.h"
#include "types.h"
#include "model_scip.h"
#include "utils.h"
#include "subproblem.h"
#include "branch_and_price.h"

#include "scip/scip.h"
#include "scip/scipdefplugins.h"

#include "pricer_csp.h"

SCIP_RETCODE runSPP (_csp &csp, _subproblem_info &subproblemInfo) {
    SCIP_VAR*  vars[subproblemInfo.journeys.size()];
    SCIP_CONS* cons[csp.N + 1];

    SCIP* reducedMasterProblem;

    buildModelScip(csp, subproblemInfo, &reducedMasterProblem, vars, cons);

    SCIPinfoMessage(reducedMasterProblem, NULL, "\nSolving...\n");

    ObjPricerCSP* csp_pricer_ptr = new ObjPricerCSP(reducedMasterProblem, CSP_PRICER_NAME, &csp, &subproblemInfo, cons);

    SCIP_CALL( SCIPincludeObjPricer(reducedMasterProblem, csp_pricer_ptr, true) );

    SCIP_CALL( SCIPactivatePricer(reducedMasterProblem, SCIPfindPricer(reducedMasterProblem, CSP_PRICER_NAME)) );

    SCIP_CALL( SCIPsolve(reducedMasterProblem) );

    if ( SCIPgetNSols(reducedMasterProblem) > 0) {
        FILE *fptr = fopen("ya.txt", "wt");
        SCIP_CALL( SCIPprintSol( reducedMasterProblem, SCIPgetBestSol( reducedMasterProblem ), fptr, FALSE) );
        fclose(fptr);

        SCIP_CALL( SCIPwriteTransProblem( reducedMasterProblem, "final.lp", NULL, FALSE));

        //SCIP_CALL( SCIPprintSol( reducedMasterProblem, SCIP( reducedMasterProblem ), NULL, FALSE) );
        //puts(".........");
        //FILE *fptr = fopen("ya.txt", "wt");
        //SCIPprintDualSol(reducedMasterProblem, fptr, TRUE);
        //SCIPprintDualSol(reducedMasterProblem, NULL, TRUE);
        //fclose(fptr);
    }

    SCIP_CALL( SCIPfreeTransform(reducedMasterProblem) );

    SCIP_CALL( SCIPfree(&reducedMasterProblem) );

    return SCIP_OKAY;
}
