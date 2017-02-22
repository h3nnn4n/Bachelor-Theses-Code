#include <iostream>

#include "random.h"
#include "reader.h"
#include "types.h"
#include "utils.h"

#include "scip/scip.h"
#include "scip/scipdefplugins.h"


SCIP_RETCODE runSPP(void) {
   return SCIP_OKAY;
}

int main(int argc, char *argv[]) {
    char input_name [256];
    char output_name[256];

    if ( argc == 1 ) {
        printf("Missing argument\n");
        return EXIT_FAILURE;
    }

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

    return 0;

    // SCIP STUFF

    //SCIP* scip;
    //// MODEL SETUP

    ////SCIP_VAR* var_journeys[nnodes];

    ////END


    //SCIP_CALL( SCIPcreate(&scip) );
    //SCIP_CALL( SCIPincludeDefaultPlugins(scip) );

    //SCIPinfoMessage(scip, NULL, "Original problem:\n");
    //SCIP_CALL( SCIPprintOrigProblem(scip, NULL, "cip", FALSE) );

    //SCIPinfoMessage(scip, NULL, "\n");
    //SCIP_CALL( SCIPpresolve(scip) );

    //SCIPinfoMessage(scip, NULL, "\nSolving...\n");
    //SCIP_CALL( SCIPsolve(scip) );

    //SCIP_CALL( SCIPfreeTransform(scip) );

    //SCIP_CALL( SCIPfree(&scip) );

    return 0;
}
