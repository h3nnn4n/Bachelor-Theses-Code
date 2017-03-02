#include <iostream>
#include <vector>

#include "types.h"
#include "utils.h"

#include "scip/scip.h"
#include "scip/scipdefplugins.h"

SCIP_RETCODE buildModel (_csp &csp, _subproblem_info &subproblemInfo, SCIP** reducedMasterProblem, SCIP_VAR** vars, SCIP_CONS** cons) {
    char name[SCIP_MAXSTRLEN];
    SCIP_CALL( SCIPcreate(reducedMasterProblem) );
    SCIP_CALL( SCIPincludeDefaultPlugins(*reducedMasterProblem) );
    SCIP_CALL( SCIPcreateProbBasic(*reducedMasterProblem, "CSP") );

    //Adds the variables
    for (int i = 0; i < (int)subproblemInfo.journeys.size(); ++i) {
        sprintf(name, "x_%d", i);
        SCIP_CALL( SCIPcreateVarBasic(*reducedMasterProblem, &vars[i], name, 0.0, 1.0, subproblemInfo.journeys[i].cost, SCIP_VARTYPE_CONTINUOUS) );

        SCIP_CALL( SCIPaddVar(*reducedMasterProblem, vars[i]) );
    }

    // Adds the contraints
    for (int i = 0; i < csp.N; ++i) {
        int non_zeros = 0;
        std::vector<SCIP_Real> lhs;
        std::vector<SCIP_VAR*>  _vars;

        for (int j = 0; j < (int)subproblemInfo.journeys.size(); ++j) {
            bool isZero = false;
            for (int k = 0; k < (int)subproblemInfo.journeys[j].covered.size(); ++k) {
                if ( subproblemInfo.journeys[j].covered[k] == i ) {
                    lhs.push_back( 1 );

                    _vars.push_back(vars[j]);
                    non_zeros ++;
                    break;
                }
            }

            if ( isZero ) {
                lhs.push_back(0);
            }
        }

        sprintf(name, "c_%d", i);

        SCIP_CALL( SCIPcreateConsBasicLinear(*reducedMasterProblem, &cons[i], name, non_zeros, &_vars[0], &lhs[0], 1.0, 1.0) );
        SCIP_CALL( SCIPaddCons(*reducedMasterProblem, cons[i]) );
    }

    SCIP_CALL( SCIPwriteOrigProblem(*reducedMasterProblem, "model.lp", NULL, 0) );

    return SCIP_OKAY;
}
