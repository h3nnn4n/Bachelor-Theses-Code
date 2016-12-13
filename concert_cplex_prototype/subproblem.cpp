#include <stdio.h>
#include "types.h"

#include <ilcplex/ilocplex.h>

void subproblem(IloNumArray reduced_costs, IloNumArray duals, _csp *t, std::vector<_journey> &journeys) {
    double adj_mat [t->N + 2][t->N + 2];
    double time_mat[t->N + 2][t->N + 2];
    double cost_mat[t->N + 2][t->N + 2];

    IloEnv   env;
    IloModel model(env);
    IloCplex cplex(model);
    IloNumVarArray var(env);
    IloRangeArray con(env);

    // Obective is to minimize
    IloObjective obj = IloMinimize(env);

    for (int i = 0; i < t->N; ++i) {
        for (int j = 0; j < t->N; ++j) {
            adj_mat [i][j] = 0;
            cost_mat[i][j] = 0;
            time_mat[i][j] = 0;
        }
    }

    for (int i = 0; i < t->N; ++i) {
        for (int j = 0; j < t->graph[i].size(); ++j) {
            //printf("%2d -> %2d\n", i, t->graph[i][j].dest);
            adj_mat [i][t->graph[i][j].dest] = 1.0;
            cost_mat[i][t->graph[i][j].dest] = t->graph[i][j].cost;
            time_mat[i][t->graph[i][j].dest] = (t->task[t->graph[i][j].dest].end_time    -
                                                t->task[t->graph[i][j].dest].start_time) +
                                               (t->task[i].end_time                      -
                                                t->task[i].start_time                  ) ;
        }
        //printf("\n");

        adj_mat[t->N][i     ] = 1.0;
        adj_mat[i   ][t->N+1] = 1.0;
    }

    // Constraint for source/sink
    // Both must be equal to 1
    {
        char n[256];
        sprintf(n, "source_flow");
        con.add(IloRange(env, 1.0, 1.0, n));
    } {
        char n[256];
        sprintf(n, "sink_flow");
        con.add(IloRange(env, 1.0, 1.0, n));
    }
    printf("Added {source,sink}_flow constraints\n");

    // Flow constraint for the vertexes (tasks)
    // Adds one constraint for each task
    for (int i = 0; i < t->N; ++i) {
        char n[256];
        sprintf(n, "task_%d", i);
        con.add(IloRange(env, 0.0, 0.0, n));
    }
    printf("Added constraint for tasks\n");

    // Adds the variables for the edges
    for (int i = 0; i < (t->N+2) * (t->N+2); ++i) {
        var.add(IloNumVar(env, 0.0, 1.0, ILOBOOL));

        char n[256];
        sprintf(n, "y_%d", i);
        var[i].setName(n);
    }
    printf("Added y_a\n");

    // Adds the vars for the vertex
    for (int i = 0; i < t->N; ++i) {
        var.add(IloNumVar(env, 0.0, 1.0, ILOBOOL));

        char n[256];
        sprintf(n, "v_%d", i);
        var[i].setName(n);
    }
    printf("Added v_a\n");

    int base = 0;

    // Populates the 0-1 matrix
    //for (int i = 0; i < t->N; ++i) {
        //con[base].setLinearCoef(
        //for (int j = 0; j < (int)journeys[i].covered.size(); ++j) {
            //printf("%d %d\n", i, journeys[i].covered[j]);
            //con[journeys[i].covered[j]-1].setLinearCoef(var[i], 1.0);
        //}
    //}
    //printf("Populated the matrix\n");



    //// Populates the 0-1 matrix
    //for (int i = 0; i < (int) journeys.size(); ++i) {
        //obj.setLinearCoef(var[i], journeys[i].cost);

        //for (int j = 0; j < (int)journeys[i].covered.size(); ++j) {
            //printf("%d %d\n", i, journeys[i].covered[j]);
            //con[journeys[i].covered[j]-1].setLinearCoef(var[i], 1.0);
        //}
    //}
    //printf("Populated the matrix\n");

    //// Configures the contrainf to the number of journeys that can be used
    //con.add(IloRange(env, 1.0, 1.0, "c_nj"));
    //for (int i = 0; i < (int) journeys.size(); ++i) {
        //con[t->N].setLinearCoef(var[i], 1.0);
    //}
    //printf("Master constraint\n");

    model.add(obj);
    model.add(con);
    model.add(var);

    cplex.exportModel("subp.lp");
}
