#include <iostream>
#include <vector>

#include "random.h"
#include "reader.h"
#include "types.h"
#include "utils.h"

#include "scip/scip.h"
#include "scip/scipdefplugins.h"


SCIP_RETCODE runSPP (int argc, char *argv[]) {
    char input_name [256];
    char output_name[256];

    sprintf(input_name , "%s.txt", argv[1]);
    sprintf(output_name, "%s.lp" , argv[1]);

    _csp csp = file_reader(input_name);
    _subproblem_info subproblemInfo;

    if ( argc == 3 ) {
        csp.n_journeys = atoi(argv[2]);
    } else {
        fprintf(stderr, "Assuming the number of journeys is equal to the number of tasks!\n");
        csp.n_journeys = csp.N;
    }

    printf("Starting\n");

    srand(666);

    std::vector<_journey> t_journeys;

    for (int i = 0; i < 2; ++i) {
        build_heur_sol ( &csp, t_journeys );
        for (auto journ : t_journeys)
            subproblemInfo.journeys.push_back(journ);

        t_journeys.clear();

        for (int i = 0; i < (int) subproblemInfo.journeys.size(); ++i) {
            subproblemInfo.usedJourneys[subproblemInfo.journeys[i].covered] = true;
        }
    }

    print_journeys(subproblemInfo.journeys);

    printf("\n");
    SCIP* scip;
    //// MODEL SETUP

    SCIP_VAR*  vars[subproblemInfo.journeys.size()];
    SCIP_CONS* cons[csp.N];

    char name[SCIP_MAXSTRLEN];
    SCIP_CALL( SCIPcreate(&scip) );
    SCIP_CALL( SCIPincludeDefaultPlugins(scip) );
    SCIP_CALL( SCIPcreateProbBasic(scip, "CSP") );

    //Adds the variables
    for (int i = 0; i < (int)subproblemInfo.journeys.size(); ++i) {
        sprintf(name, "x_%d", i);
        SCIP_CALL( SCIPcreateVarBasic(scip, &vars[i], name, 0.0, 1.0, subproblemInfo.journeys[i].cost, SCIP_VARTYPE_BINARY) );

        SCIP_CALL( SCIPaddVar(scip, vars[i]) );
    }

    for (int i = 0; i < csp.N; ++i) {
        int non_zeros = 0;
        std::vector<SCIP_Real> lhs;

        for (int j = 0; j < (int)subproblemInfo.journeys.size(); ++j) {
            bool isZero = false;
            for (int k = 0; k < (int)subproblemInfo.journeys[j].covered.size(); ++k) {
                if ( subproblemInfo.journeys[j].covered[k] == i ) {
                    lhs.push_back(1);
                    non_zeros ++;
                    break;
                }
            }

            if ( isZero ) {
                lhs.push_back(0);
            }
        }

        sprintf(name, "c_%d", i);

        SCIP_CALL( SCIPcreateConsBasicLinear(scip, &cons[i], name, non_zeros, vars, &lhs[0], 1.0, 1.0) );
        SCIP_CALL( SCIPaddCons(scip, cons[i]) );
    }

    SCIP_CALL( SCIPwriteOrigProblem(scip, "model.lp", NULL, 0) );

    SCIP_CALL( SCIPpresolve(scip) );

    SCIPinfoMessage(scip, NULL, "\nSolving...\n");
    SCIP_CALL( SCIPsolve(scip) );

    SCIP_CALL( SCIPfreeTransform(scip) );

    SCIP_CALL( SCIPfree(&scip) );

    return SCIP_OKAY;
}

int main(int argc, char *argv[]) {
    if ( argc == 1 ) {
        printf("Missing argument\n");
        return EXIT_FAILURE;
    }

    SCIP_RETCODE retcode;

    retcode = runSPP(argc, argv);

    if(retcode != SCIP_OKAY) {
        SCIPprintError(retcode);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
