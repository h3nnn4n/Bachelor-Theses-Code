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

        if ( argc != 2 ) {
            printf("Missing argument\n");
            return EXIT_FAILURE; }

        sprintf(input_name , "%s.txt", argv[1]);
        sprintf(output_name, "%s.lp" , argv[1]);

        _csp t = file_reader(input_name);
        std::vector<_journey> journeys;

        srand(time(NULL));
        //srand(666);

        journeys.push_back(all_powerful_journey(&t));

        //for (int i = 0; i < 5; ++i) {
            //journeys.push_back(random_journey(&t));
        //}

        print_journeys(journeys);
        printf("\n");
        //return 0;

        // begins model construction
        IloNumVarArray var(env);
        IloRangeArray con(env);

        populate_model(model, var, con, &t, journeys) ;

        IloCplex cplex(model);
        cplex.exportModel("lpex1.lp");

        // Optimize the problem and obtain solution for the first run.
        if ( !cplex.solve() ) {
            env.error() << "Failed to optimize LP" << endl;
            throw(-1);
        }

        // Solves the subproblem for the first time to get the
        // reduced cost and begin the column generation process
        IloNumArray red_cost(env);
        IloNumArray duals(env);

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

        cplex.getReducedCosts(red_cost, var);
        cplex.getDuals(duals, con);
        subproblem(red_cost, duals, &t, journeys);

        //while ( 1 ) {

        //}

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
