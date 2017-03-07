#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <time.h>
#include <unistd.h>

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
#include "branch_and_price.h"
#include "perf_data.h"
#include "time_keeper.h"

#include <ilcplex/ilocplex.h>

_perf_data perf_data;

ILOSTLBEGIN

int main (int argc, char **argv) {
    IloEnv   env;
    try {
        struct timespec total_time_t1;
        struct timespec total_time_t2;
        timekeeper_tic(&total_time_t1);

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

        perf_data_init(&perf_data);

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
        struct timespec init_heur_t1;
        struct timespec init_heur_t2;

        timekeeper_tic(&init_heur_t1);

        bool found_feasible_sol = false;
        std::vector<_journey> t_journeys;
        for (int i = 0; i < 2; ++i) {
            if ( build_heur_sol ( &csp, t_journeys ) ) {
                found_feasible_sol = true;
            }
            update_used_journeys_with_vector(subproblemInfo, t_journeys);

            t_journeys.clear();
        }

        if ( !found_feasible_sol ) {
            fprintf(stderr, "Could not find a feasible solution\n");
            fprintf(stderr, "Using a infeasible journey that covers everything\n");

            _journey journey = all_powerful_journey(&csp);
            subproblemInfo.using_all_powerful_journey = true;
            update_used_journeys(subproblemInfo, journey);

            //exit(-1);
        }

        init_subproblem_info( &subproblemInfo, &csp );

        if ( !subproblemInfo.using_all_powerful_journey) {
            for (int i = 0; i < (int) subproblemInfo.journeys.size(); ++i) {
                validateJourney(&subproblemInfo, subproblemInfo.journeys[i]);
            }
        }

        timekeeper_tic(&init_heur_t2);

        perf_data.initial_heur_time = time_diff_double( init_heur_t1, init_heur_t2 );

        print_journeys(subproblemInfo.journeys);
        printf("\n");

        perf_data_show(&perf_data);
        //exit(0);

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

        //goto skip;
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
//skip:

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

        bool fractional_results = false;
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

        fractional_results = total_columns_used != csp.n_journeys;

        printf("%4d columns where used, expected %4d\n", total_columns_used, csp.n_journeys);

        //fractional_results = true;

        // If there are more selected columns than the specified then we have
        // a fractional solution and we need to do some branch and price
        if ( fractional_results ) {
            SCIP_RETCODE retcode;

            retcode = runSPP(csp, subproblemInfo);

            if(retcode != SCIP_OKAY) {
                SCIPprintError(retcode);
                return EXIT_FAILURE;
            } else {
            }
        }

        timekeeper_tic(&total_time_t2);

        perf_data.total_time = time_diff_double( total_time_t1, total_time_t2 );

        perf_data_show(&perf_data);
    }
    catch (IloException& e) {
        cerr << "Concert exception caught: " << e << endl;
    }
    catch (...) {
        cerr << "Unknown exception caught" << endl;
    }

    env.end();

    return EXIT_SUCCESS;
}
