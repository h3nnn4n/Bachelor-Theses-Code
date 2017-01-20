#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstdio>

#include <map>

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
        IloModel model(env);

        char input_name [256];
        char output_name[256];

        if ( argc == 1 ) {
            printf("Missing argument\n");
            return EXIT_FAILURE;
        }

        sprintf(input_name , "%s.txt", argv[1]);
        sprintf(output_name, "%s.lp" , argv[1]);

        _csp t = file_reader(input_name);
        std::vector<_journey> journeys;

        if ( argc == 3 ) {
            t.n_journeys = atoi(argv[2]);
        } else {
            fprintf(stderr, "Assuming the number of journeys is equal to the number of tasks!\n");
            t.n_journeys = t.N;
        }

        printf("Starting\n");

        srand(time(NULL));
        //srand(666);

        // Prints the graph (it indexes from zero)
        //for (int i = 0; i < (int)t.graph.size(); ++i) {
            //printf("%3d -> ", i);
            //for (int j = 0; j < (int)t.graph[i].size(); ++j) {
                //printf("%3d ", t.graph[i][j].dest);
            //}
            //printf("\n");
        //}

        build_heur_sol ( &t, journeys );
        //journeys.push_back(all_powerful_journey(&t));

        //for (int i = 0; i < 5; ++i) {
            //journeys.push_back(random_journey(&t));
        //}

        print_journeys(journeys);
        printf("\n");
        //return 0;

        // begins model construction
        IloNumVarArray var(env);
        IloRangeArray con(env);

        populate_model(model, var, con, &t, journeys, &t) ;

        IloCplex cplex(model);
        cplex.exportModel("lpex0.lp");

        cplex.setOut(env.getNullStream());
        // Optimize the problem and obtain solution for the first run.
        if ( !cplex.solve() ) {
            env.error() << "Failed to optimize LP" << endl;
            throw(-1);
        }

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
        //

        double reduced_cost;

        int cont = 0;

        do {
            // Solves the master problem
            if ( !cplex.solve() ) {
                env.error() << "Failed to optimize LP" << endl;
                throw(-1);
            }

            // Solves the subproblem
            IloNumArray red_cost(env);
            IloNumArray duals(env);
            cplex.getReducedCosts(red_cost, var);
            cplex.getDuals(duals, con);

            _journey new_journey = subproblem(red_cost, duals, &t, journeys, &reduced_cost);

            journeys.push_back(new_journey);

            // Gets a reference to the objective function and adds the new coef for the column
            //IloObjective obj = IloMinimize(env);
            IloObjective obj = IloMinimize(env);

            // Create a new expression to build the new column
            //puts("a");
            //IloNumColumn col;
            IloNumColumn col = obj(new_journey.cost);
            printf(" cost = %d\n", new_journey.cost);
            //IloNumExpr expr(env);
            //obj.add(obj(new_journey.cost));

            //puts("0");
            //col += obj(new_journey.cost);
            //puts("1");

            // Walks throught the new journey and builds the coeffs for the column
            for (int i = 0; i < (int)new_journey.covered.size(); ++i) {
                //expr +=
                printf("%d %d\n", i, new_journey.covered[i]);
                col += con[new_journey.covered[i]](1.0);
            }

            col += con[t.N](1.0);

            //model.add(expr <= t->time_limit);
            //var.add(IloNumVar(end, 0.0, 1.0, ILOFLOAT));
            //var.add(IloNumVar( expr ));

            var.add(IloNumVar(obj(new_journey.cost) + col, 0.0, 1.0, ILOFLOAT));
            //obj.setLinearCoef(var[(int)journeys.size()], new_journey.cost);
            model.add(var);
            //model.add(obj);
            //puts("3");
            //expr.end();

            //if ( cont++ > 20 ) reduced_cost = 0;

            //switch ( cont ) {
                //case 1:
                    //cplex.exportModel("lpex1.lp");
                    //break;
                //case 2:
                    //cplex.exportModel("lpex2.lp");
                    //break;
                //case 3:
                    cplex.exportModel("lpex3.lp");
                    //break;
                //default:
                    //break;
            //}

            printf(" - %d %f\n", cont, reduced_cost);
        } while ( reduced_cost != 0 );

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
