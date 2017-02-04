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
#include "model.h"
#include "subproblem.h"

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

        ////Prints the graph (it indexes from zero)
        //for (int i = 0; i < (int)csp.graph.size(); ++i) {
            //printf("%3d -> ", i);
            //for (int j = 0; j < (int)csp.graph[i].size(); ++j) {
                //printf("(%3d, %4d) ", csp.graph[i][j].dest, csp.graph[i][j].cost);
            //}
            //printf("\n");
        //}

        build_heur_sol ( &csp, subproblemInfo.journeys );

        for (int i = 0; i < (int) subproblemInfo.journeys.size(); ++i) {
            subproblemInfo.usedJourneys[subproblemInfo.journeys[i].covered] = true;
        }

        //journeys.push_back(all_powerful_journey(&t));

        //for (int i = 0; i < 5; ++i) {
            //journeys.push_back(random_journey(&t));
        //}

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

        //populate_model(model, var, con, &t, journeys, &t) ;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // Obective is to minimize
        IloObjective obj = IloMinimize(env);

        // Adds one constraint for each task
        for (int i = 0; i < csp.N; ++i) {
            char n[256];
            sprintf(n, "c%d", i);
            con.add(IloRange(env, 1.0, 1.0, n));
        }
        //printf("Added constraint\n");

        // Adds one variable for each existing journey
        for (int i = 0; i < (int) subproblemInfo.journeys.size(); ++i) {
            var.add(IloNumVar(env, 0.0, 1.0, ILOFLOAT));

            char n[256];
            sprintf(n, "x%d", i);
            var[i].setName(n);
        }
        //printf("Added vars\n");

        // Populates the 0-1 matrix
        for (int i = 0; i < (int) subproblemInfo.journeys.size(); ++i) {
            obj.setLinearCoef(var[i], subproblemInfo.journeys[i].cost);

            for (int j = 0; j < (int)subproblemInfo.journeys[i].covered.size(); ++j) {
                //printf("%d %d\n", i, journeys[i].covered[j]);
                con[subproblemInfo.journeys[i].covered[j]].setLinearCoef(var[i], 1.0);
            }
        }
        //printf("Populated the matrix\n");

        // Configures the contrainf to the number of journeys that can be used
        con.add(IloRange(env, (float)csp.n_journeys, (float)csp.n_journeys, "c_nj"));
        for (int i = 0; i < (int) subproblemInfo.journeys.size(); ++i) {
            con[csp.N].setLinearCoef(var[i], 1.0);
        }
        //printf("Master constraint\n");

        model.add(obj);
        model.add(con);
        model.add(var);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        IloCplex cplex(model);
        cplex.exportModel("lpex0.lp");

        //cplex.setOut(env.getNullStream());
        // Optimize the problem and obtain solution for the first run.
        //if ( !cplex.solve() ) {
            //env.error() << "Failed to optimize LP" << endl;
            //throw(-1);
        //}

        //{
        //IloNumArray vals(env);
        //env.out() << "Solution status = " << cplex.getStatus() << endl;
        //env.out() << "Solution value  = " << cplex.getObjValue() << endl;
        //cplex.getValues(vals, var);
        //env.out() << "Values        = " << vals << endl;
        //cplex.getSlacks(vals, con);
        //env.out() << "Slacks        = " << vals << endl;
        //cplex.getDuals(vals, con);
        //env.out() << "Duals         = " << vals << endl;
        //cplex.getReducedCosts(vals, var);
        //env.out() << "Reduced Costs = " << vals << endl;
        //cout << "\n\n";
        //}

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

            //IloNumArray vals(env);
            env.out() << "RMP status = " << cplex.getStatus() << " " << " value  = " << cplex.getObjValue() << endl;
            //cplex.getValues(vals, var);
            //env.out() << "Values        = " << vals << endl;
            //cplex.getSlacks(vals, con);
            //env.out() << "Slacks        = " << vals << endl;
            //cplex.getDuals(vals, con);
            //env.out() << "Duals         = " << vals << endl;
            //cplex.getReducedCosts(vals, var);
            //env.out() << "Reduced Costs = " << vals << endl;
            //cout << "\n\n";

            // Solves the subproblem
            IloNumArray red_cost(env);
            IloNumArray duals(env);
            cplex.getDuals(duals, con);

            update_subproblem_duals( &subproblemInfo, &csp, duals);
            //_journey new_journey;
            _journey new_journey = subproblem(duals, &csp, &subproblemInfo, &reduced_cost);
            validateJourney(&subproblemInfo, new_journey);
            //_journey new_journey = subproblem(duals, &t, journeys, &reduced_cost, usedJourneys);

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

            //char name[256];
            //sprintf(name, "rmp_%04d.lp", cont);
            //cplex.exportModel(name);

            //if ( cont > 2 ) break;

            printf(" Iterations %4d reduced cost = %5.2f\n\n", cont, reduced_cost);
        } while ( reduced_cost != 0 );

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
