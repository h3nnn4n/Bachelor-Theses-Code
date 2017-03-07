#include <iostream>
#include <vector>

#include <cstring>

#include "types.h"
#include "utils.h"

#include "model_scip.h"

#include "scip/scip.h"
#include "scip/scipdefplugins.h"

SCIP_RETCODE buildModelScip (_csp &csp, _subproblem_info &subproblemInfo, SCIP** reducedMasterProblem, SCIP_VAR** vars, SCIP_CONS** cons) {
    char name[SCIP_MAXSTRLEN];
    SCIP_CALL( SCIPcreate(reducedMasterProblem) );
    SCIP_CALL( SCIPincludeDefaultPlugins(*reducedMasterProblem) );

    SCIP_CALL( SCIPsetBoolParam(*reducedMasterProblem, "lp/presolving", FALSE) );

    SCIP_CALL( SCIPsetSeparating(*reducedMasterProblem, SCIP_PARAMSETTING_OFF, true) );   //disable cuts
    SCIP_CALL( SCIPsetHeuristics(*reducedMasterProblem, SCIP_PARAMSETTING_OFF, true) );   //disable heuristics
    SCIP_CALL( SCIPsetPresolving(*reducedMasterProblem, SCIP_PARAMSETTING_OFF, true) );   //disable presolving

    SCIPsetBoolParam(*reducedMasterProblem, "lp/presolving", FALSE);

    SCIPsetIntParam(*reducedMasterProblem, "propagating/maxrounds", 0);
    SCIPsetIntParam(*reducedMasterProblem, "propagating/maxroundsroot", 0);

    SCIP_CALL( SCIPcreateProbBasic(*reducedMasterProblem, "CSP") );

    //Adds the variables
    for (int i = 0; i < (int)subproblemInfo.journeys.size(); ++i) {
        char tmp_name[255];
        char var_name[255];
        (void) SCIPsnprintf(var_name, 255, "journey_");
        for (std::vector<int>::const_iterator it = subproblemInfo.journeys[i].covered.begin(); it != subproblemInfo.journeys[i].covered.end(); ++it) {
            strncpy(tmp_name, var_name, 255);
            (void) SCIPsnprintf(var_name, 255, "%s_%d", tmp_name, *it);
        }
        SCIPinfoMessage(*reducedMasterProblem, NULL, "new variable <%s>\n", var_name);

        SCIP_CALL( SCIPcreateVarBasic(*reducedMasterProblem, &vars[i], var_name, 0.0, SCIPinfinity(*reducedMasterProblem),
                    subproblemInfo.journeys[i].cost, SCIP_VARTYPE_INTEGER) );

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

        SCIP_CALL( SCIPcreateConsLinear(*reducedMasterProblem, &cons[i], name, non_zeros, &_vars[0], &lhs[0], 1.0, 1.0,
                   true,                   /* initial */
                   false,                  /* separate */
                   true,                   /* enforce */
                   true,                   /* check */
                   false,                   /* propagate */
                   false,                  /* local */
                   true,                   /* modifiable */
                   false,                  /* dynamic */
                   false,                  /* removable */
                   false) );               /* stickingatnode */
        SCIP_CALL( SCIPaddCons(*reducedMasterProblem, cons[i]) );
    }

    sprintf(name, "mi");

    SCIP_Real lhs[(int)subproblemInfo.journeys.size()];
    for (int i = 0; i < (int)subproblemInfo.journeys.size(); ++i) {
        lhs[i] = 1.0;
    }

    SCIP_CALL( SCIPcreateConsLinear(*reducedMasterProblem, // Model
               &cons[csp.N], // Contraint
               name, // Name
               (int)subproblemInfo.journeys.size(), // nvars
               vars,                   /* vars */
               lhs,                    /* vals */
               csp.n_journeys,         /* lhs */
               csp.n_journeys,         /* rhs */
               true,                   /* initial */
               false,                  /* separate */
               true,                   /* enforce */
               true,                   /* check */
               false,                   /* propagate */
               false,                  /* local */
               true,                   /* modifiable */
               false,                  /* dynamic */
               false,                  /* removable */
               false) );               /* stickingatnode */
    SCIP_CALL( SCIPaddCons(*reducedMasterProblem, cons[csp.N]) );

    SCIP_CALL( SCIPwriteOrigProblem(*reducedMasterProblem, "model.lp", NULL, 0) );

    return SCIP_OKAY;
}

