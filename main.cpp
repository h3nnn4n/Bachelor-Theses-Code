#include <iostream>
#include <vector>

#include <time.h>

#include "random.h"
#include "reader.h"
#include "types.h"
#include "model.h"
#include "utils.h"

#include "scip/scip.h"
#include "scip/scipdefplugins.h"

//#include "pricer_csp.h"
#include "pricer_vrp.h"

static const char* VRP_PRICER_NAME = "VRP_Pricer";

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
    //srand(time(NULL));

    std::vector<_journey> t_journeys;

    for (int i = 0; i < 1; ++i) {
        build_heur_sol ( &csp, t_journeys );
        for (auto journ : t_journeys)
            subproblemInfo.journeys.push_back(journ);

        t_journeys.clear();

        for (int i = 0; i < (int) subproblemInfo.journeys.size(); ++i) {
            subproblemInfo.usedJourneys[subproblemInfo.journeys[i].covered] = true;
        }
    }

    //print_journeys(subproblemInfo.journeys);

    printf("\n");

    SCIP_VAR*  vars[subproblemInfo.journeys.size()];
    SCIP_CONS* cons[csp.N + 1];
    //SCIP_ROW** rows;
    //int nrows;

    SCIP* reducedMasterProblem;

    buildModel(csp, subproblemInfo, &reducedMasterProblem, vars, cons);

    //SCIP_CALL( SCIPpresolve(reducedMasterProblem) );
    SCIPinfoMessage(reducedMasterProblem, NULL, "\nSolving...\n");

    int cont = 0;

    //SCIP_Real dualVariables[csp.N];

    /* include VRP pricer */
    //ObjPricerVRP* vrp_pricer_ptr = new ObjPricerVRP(scip, VRP_PRICER_NAME, num_nodes, capacity, demand, dist,
        //arc_var, arc_con, part_con);

    //SCIP_CALL( SCIPincludeObjPricer(scip, vrp_pricer_ptr, true) );

    //SCIP_CALL( SCIPincludePricerXyz( reducedMasterProblem ) );

   ObjPricerVRP* vrp_pricer_ptr = new ObjPricerVRP(reducedMasterProblem, VRP_PRICER_NAME, &csp, &subproblemInfo, cons);

   SCIP_CALL( SCIPincludeObjPricer(reducedMasterProblem, vrp_pricer_ptr, true) );

   SCIP_CALL( SCIPactivatePricer(reducedMasterProblem, SCIPfindPricer(reducedMasterProblem, VRP_PRICER_NAME)) );

   //SCIP_CALL( SCIPwriteOrigProblem(scip, "vrp_init.lp", "lp", FALSE) );

    do {
        puts("\nBEGIN");
        cont ++;

        SCIP_CALL( SCIPsolve(reducedMasterProblem) );

        //SCIP_CALL( SCIPgetLPRowsData(reducedMasterProblem, &rows, &nrows) );
        //rows = SCIPgetLPRows(reducedMasterProblem);

        //printf("problem has %d rows\n", csp.N);
        //for (int i = 0; i < csp.N; ++i) {
            //dualVariables[i] = SCIPgetDualsolLinear(reducedMasterProblem, cons[i]);
            //printf("%4d %2.2f\n", i, dualVariables[i]);
            //printf("%4d %2.2f\n", i, SCIPgetDualsolLinear(reducedMasterProblem, cons[i]));
            //SCIP_Real dual = SCIPgetDualsolLinear(reducedMasterProblem, cons[i]);
        //}
        //puts("END");

    } while ( cont < 1 );

   if ( SCIPgetNSols(reducedMasterProblem) > 0) {
        //SCIP_CALL( SCIPprintSol( reducedMasterProblem, SCIPgetBestSol( reducedMasterProblem ), NULL, FALSE) );
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
