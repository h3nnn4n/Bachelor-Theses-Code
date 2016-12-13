#include <ilcplex/ilocplex.h>

#include "types.h"

void  populate_model (IloModel model, IloNumVarArray var, IloRangeArray con, _csp *t, std::vector<_journey> const &journeys) {
    IloEnv env = model.getEnv();

    // Obective is to minimize
    IloObjective obj = IloMinimize(env);

    // Adds one constraint for each task
    for (int i = 0; i < t->N; ++i) {
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
        con[t->N].setLinearCoef(var[i], 1.0);
    }

    model.add(obj);
    model.add(con);
    model.add(var);
}
