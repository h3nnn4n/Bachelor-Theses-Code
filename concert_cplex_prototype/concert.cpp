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
            return EXIT_FAILURE;
        }

        sprintf(input_name , "%s.txt", argv[1]);
        sprintf(output_name, "%s.lp" , argv[1]);

        _csp t = file_reader(input_name);
        std::vector<_journey> journeys;

        //srand(time(NULL));
        srand(666);

        journeys.push_back(all_powerful_journey(&t));

        print_journeys(journeys);
        printf("\n");
        //return 0;

        // begins model construction
        IloNumVarArray var(env);
        IloRangeArray con(env);

        // Obective is to minimize
        IloObjective obj = IloMinimize(env);

        // Adds one constraint for each task
        for (int i = 0; i < t.N; ++i) {
            char n[256];
            sprintf(n, "c%d", i);
            con.add(IloRange(env, 1.0, 1.0, n));
        }

        // Adds one variable for each existing journey
        for (int i = 0; i < (int) journeys.size(); ++i) {
            var.add(IloNumVar(env, 0.0, 1.0, ILOFLOAT));

            char n[256];
            sprintf(n, "x%d", i);
            var[i].setName(n);
        }

        // Populates the 0-1 matrix
        for (int i = 0; i < (int) journeys.size(); ++i) {
            obj.setLinearCoef(var[i], journeys[i].cost);

            for (int j = 0; j < (int)journeys[i].covered.size(); ++j) {
               printf("%d %d\n", i, journeys[i].covered[j]);
               con[journeys[i].covered[j]].setLinearCoef(var[i], 1.0);
            }
        }

        // Configures the contrainf to the number of journeys that can be used
        con.add(IloRange(env, 1.0, 1.0, "c_nj"));
        for (int i = 0; i < (int) journeys.size(); ++i) {
            con[t.N].setLinearCoef(var[i], 1.0);
        }

        model.add(obj);
        model.add(con);
        model.add(var);

        //populatebyrow (model, var, con);

        IloCplex cplex(model);
        cplex.exportModel("lpex1.lp");

        // Optimize the problem and obtain solution.
        if ( !cplex.solve() ) {
            env.error() << "Failed to optimize LP" << endl;
            throw(-1);
        }

        IloNumArray vals(env);
        env.out() << "Solution status = " << cplex.getStatus() << endl;
        env.out() << "Solution value  = " << cplex.getObjValue() << endl;
        cplex.getValues(vals, var);
        env.out() << "Values        = " << vals << endl;
        cplex.getSlacks(vals, con);
        env.out() << "Slacks        = " << vals << endl;
        cplex.getDuals(vals, con);
        env.out() << "Duals         = " << vals << endl;
        cplex.getReducedCosts(vals, var);
        env.out() << "Reduced Costs = " << vals << endl;
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


// To populate by nonzero, we first create the rows, then create the
// columns, and then change the nonzeros of the matrix 1 at a time.

    static void
populatebynonzero (IloModel model, IloNumVarArray x, IloRangeArray c)
{
    IloEnv env = model.getEnv();

    IloObjective obj = IloMaximize(env);
    c.add(IloRange(env, -IloInfinity, 20.0));
    c.add(IloRange(env, -IloInfinity, 30.0));

    x.add(IloNumVar(env, 0.0, 40.0));
    x.add(IloNumVar(env));
    x.add(IloNumVar(env));

    obj.setLinearCoef(x[0], 1.0);
    obj.setLinearCoef(x[1], 2.0);
    obj.setLinearCoef(x[2], 3.0);

    c[0].setLinearCoef(x[0], -1.0);
    c[0].setLinearCoef(x[1],  1.0);
    c[0].setLinearCoef(x[2],  1.0);
    c[1].setLinearCoef(x[0],  1.0);
    c[1].setLinearCoef(x[1], -3.0);
    c[1].setLinearCoef(x[2],  1.0);

    c[0].setName("c1");
    c[1].setName("c2");

    x[0].setName("x1");
    x[1].setName("x2");
    x[2].setName("x3");

    model.add(obj);
    model.add(c);
}  // END populatebynonzero
