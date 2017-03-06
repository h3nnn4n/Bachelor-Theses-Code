#include <iostream>
#include <cstdlib>
#include <cstdio>

#include <map>
#include <vector>

#include "reader.h"
#include "types.h"
#include "printer_to_graphviz.h"
#include "backtrack.h"
#include "utils.h"
#include "random.h"
#include "model_cplex.h"
#include "model_scip.h"
#include "subproblem.h"
#include "model_scip.h"
#include "exact_subproblem.h"

#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

int main (int argc, char **argv) {
    IloEnv   env;
    try {
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

        //srand(time(NULL));
        srand(666);

        // generation for the first set of columns
        std::vector<_journey> t_journeys;
        for (int i = 0; i < 5; ++i) {
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

        init_subproblem_info( &subproblemInfo, &csp );

        for (int i = 0; i < (int) subproblemInfo.journeys.size(); ++i) {
            validateJourney(&subproblemInfo, subproblemInfo.journeys[i]);
        }

        IloModel model(env);

        // begins model construction
        IloNumVarArray var(env);
        IloRangeArray con(env);
        IloObjective obj = IloMinimize(env);

        buildModelCplex(model, var, con, obj, subproblemInfo.journeys, &csp, &subproblemInfo) ;

        IloCplex cplex(model);
        //cplex.exportModel("lpex0.lp");

        double reduced_cost;

        int cont = 0;

        do {
            cont ++;
            // Solves the master problem
            cplex.setOut(env.getNullStream());
            if ( !cplex.solve() ) {
                env.error() << "Failed to optimize LP" << endl;
                throw(-1);
            }

            env.out() << "RMP status = " << cplex.getStatus() << " " << " value  = " << cplex.getObjValue() << endl;

            // Solves the subproblem
            IloNumArray red_cost(env);
            IloNumArray duals(env);
            cplex.getDuals(duals, con);

            update_subproblem_duals( &subproblemInfo, &csp, duals);
            //_journey new_journey;
            _journey new_journey = subproblem(&csp, &subproblemInfo, &reduced_cost);
            validateJourney(&subproblemInfo, new_journey);

            subproblemInfo.journeys.push_back(new_journey);
            subproblemInfo.usedJourneys[new_journey.covered] = true;

            // Create a new expression to build the new column
            IloNumColumn col = obj(new_journey.cost);
            printf("New Column cost = %4d  time = %4d  Covered [", new_journey.cost, new_journey.time);

            // Walks throught the new journey and builds the coeffs for the column
            for (int i = 0; i < (int)new_journey.covered.size(); ++i) {
                printf("%4d, ", new_journey.covered[i]);
                col += con[new_journey.covered[i]](1.0);
            }

            printf("\b\b]\n");

            col += con[csp.N](1.0);

            var.add( IloNumVar(col + obj(new_journey.cost), 0.0, 1.0, ILOFLOAT));
            model.add(var);

            printf(" Iterations %4d reduced cost = %5.2f\n\n", cont, reduced_cost);
        } while ( reduced_cost < 0 );

        printf("RMP has cost = %5.2f, stopping\n", reduced_cost);


        cplex.setOut(env.getNullStream());
        // Optimize the problem with the final set of columns
        if ( !cplex.solve() ) {
            env.error() << "Failed to optimize LP" << endl;
            throw(-1);
        }

        IloNumArray vals(env);
        env.out() << "Master Problem Status = " << cplex.getStatus() <<  " value  = " << cplex.getObjValue() << endl;
        cplex.getValues(vals, var);

        int total_columns_used = 0;
        for (int i = 0; i < (int) subproblemInfo.journeys.size(); ++i) {
            if ( vals[i] ) {
                total_columns_used++;
                printf("x[%3d] = %2.18f ", i, vals[i]);
                printf(" cost = %4d  time = %4d  [", subproblemInfo.journeys[i].cost, subproblemInfo.journeys[i].time);
                for (int j = 0; j < (int)subproblemInfo.journeys[i].covered.size(); ++j) {
                    printf("%4d, ", subproblemInfo.journeys[i].covered[j]);
                }
                printf("\b\b]\n");
            }
        }

        //IloModel model_final(env);
        //model_final.add(model);
        //model_final.add(IloConversion(env, var, ILOINT));

        //IloCplex       cplex_final(model_final);

        //cplex_final.setOut(env.getNullStream());
        //// Optimize the problem with the final set of columns
        //if ( !cplex_final.solve() ) {
            //env.error() << "Failed to optimize LP" << endl;
            //throw(-1);
        //}

        //IloNumArray vals(env);
        //env.out() << "Master Problem Status = " << cplex_final.getStatus() <<  " value  = " << cplex_final.getObjValue() << endl;
        //cplex_final.getValues(vals, var);

        //int total_columns_used = 0;
        //for (int i = 0; i < (int) journeys.size(); ++i) {
            //if ( vals[i] ) {
                //total_columns_used++;
                //printf("x[%3d] = %2.18f ", i, vals[i]);
                //printf(" cost = %4d  time = %4d  [", journeys[i].cost, journeys[i].time);
                //for (int j = 0; j < (int)journeys[i].covered.size(); ++j) {
                    //printf("%4d, ", journeys[i].covered[j]);
                //}
                //printf("\b\b]\n");
            //}
        //}

        printf("%4d columns where used, expected %4d\n", total_columns_used, csp.n_journeys);
    }
    catch (IloException& e) {
        cerr << "Concert exception caught: " << e << endl;
    }
    catch (...) {
        cerr << "Unknown exception caught" << endl;
    }

    env.end();

    return 0;
}
