#include <ilcplex/ilocplex.h>

#include "types.h"

void  buildModelCplex (IloModel model, IloNumVarArray var, IloRangeArray con, IloObjective obj, std::vector<_journey> const &journeys, _csp *csp, _subproblem_info *subproblemInfo) {
    IloEnv env = model.getEnv();

    // Adds one constraint for each task
    for (int i = 0; i < csp->N; ++i) {
        char n[256];
        sprintf(n, "c%d", i);
        con.add(IloRange(env, 1.0, 1.0, n));
    }
    //printf("Added constraint\n");

    // Adds one variable for each existing journey
    for (int i = 0; i < (int) subproblemInfo->journeys.size(); ++i) {
        var.add(IloNumVar(env, 0.0, 1.0, ILOFLOAT));

        char n[256];
        sprintf(n, "x%d", i);
        var[i].setName(n);
    }
    //printf("Added vars\n");

    // Populates the 0-1 matrix
    for (int i = 0; i < (int) subproblemInfo->journeys.size(); ++i) {
        obj.setLinearCoef(var[i], subproblemInfo->journeys[i].cost);

        for (int j = 0; j < (int)subproblemInfo->journeys[i].covered.size(); ++j) {
            //printf("%d %d\n", i, journeys[i].covered[j]);
            con[subproblemInfo->journeys[i].covered[j]].setLinearCoef(var[i], 1.0);
        }
    }
    //printf("Populated the matrix\n");

    // Configures the contrainf to the number of journeys that can be used
    con.add(IloRange(env, (float)csp->n_journeys, (float)csp->n_journeys, "c_nj"));
    for (int i = 0; i < (int) subproblemInfo->journeys.size(); ++i) {
        con[csp->N].setLinearCoef(var[i], 1.0);
    }
    //printf("Master constraint\n");

    model.add(obj);
    model.add(con);
    model.add(var);
}
