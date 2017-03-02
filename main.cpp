#include <iostream>
#include <vector>

#include "random.h"
#include "reader.h"
#include "types.h"
#include "model.h"
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

    SCIP_VAR*  vars[subproblemInfo.journeys.size()];
    SCIP_CONS* cons[csp.N];

    SCIP* reducedMasterProblem;

    buildModel(csp, subproblemInfo, &reducedMasterProblem, vars, cons);

    SCIP_CALL( SCIPpresolve(reducedMasterProblem) );

    SCIPinfoMessage(reducedMasterProblem, NULL, "\nSolving...\n");
    SCIP_CALL( SCIPsolve(reducedMasterProblem) );

    SCIP_CALL( SCIPfreeTransform(reducedMasterProblem) );

    SCIP_CALL( SCIPfree(&reducedMasterProblem) );

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
