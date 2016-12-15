#include <iostream>
#include <stdio.h>
#include "types.h"

#include <ilcplex/ilocplex.h>
#include <ilcp/cpext.h>

//#define __show_steps_subp

using namespace std;

void subproblem(IloNumArray reduced_costs, IloNumArray duals, _csp *t, std::vector<_journey> &journeys) {
    double adj_mat [t->N + 2][t->N + 2];
    double time_mat[t->N + 2][t->N + 2];
    double cost_mat[t->N + 2][t->N + 2];

    IloEnv         env         ;
    IloModel       model(env  );
    IloCplex       cplex(model);

    for (int i = 0; i < t->N+2; ++i) {
        for (int j = 0; j < t->N+2; ++j) {
            adj_mat [i][j] = 0;
            cost_mat[i][j] = 0;
            time_mat[i][j] = 0;
        }
    }

    // This populates an adjacency matrix, cost matrix and a time matrix
#ifdef __show_steps_subp
    printf("%d\n", (int)t->graph.size());
#endif
    for (int i = 0; i < (int)t->graph.size(); ++i) {
        for (int j = 0; j < (int)t->graph[i].size(); ++j) {
#ifdef __show_steps_subp
            printf("%2d -> %2d\n", i, t->graph[i][j].dest);
#endif
            adj_mat [i][t->graph[i][j].dest] = 1.0;
            cost_mat[i][t->graph[i][j].dest] = t->graph[i][j].cost;
            time_mat[i][t->graph[i][j].dest] = (t->task[t->graph[i][j].dest].end_time    -
                                                t->task[t->graph[i][j].dest].start_time) +
                                               (t->task[i].end_time                      -
                                                t->task[i].start_time                  ) ;
        }
#ifdef __show_steps_subp
        printf("\n");
#endif
    }

    // Adds the 2 virtual nodes
    for (int i = 0; i < t->N; ++i) {
        adj_mat[t->N][i     ] = 1.0;
        adj_mat[i   ][t->N+1] = 1.0;
    }

    // Print the matrix, for debugging purposes
#ifdef __show_steps_subp
    for (int i = 0; i < t->N+2; ++i) {
        for (int j = 0; j < t->N+2; ++j) {
            printf("%2.1f ", adj_mat[i][j]);
            //printf("%2.1f ", time_mat[i][j]);
            //printf("%2.1f ", cost_mat[i][j]);
        }
        printf("\n");
    }
#endif

    // Adds the variables for the edges
    // Makes a (N+2) x (N+2) matrix
    IloArray<IloNumVarArray> y(env, t->N+2);
    for (int i = 0; i < t->N+2; ++i) {
        y[i] = IloNumVarArray(env, t->N+2, 0, 1);
    }

    // Set the variable names. This is mostly usefull
    // for debugging the model and reading the lp output
    for (int i = 0; i < t->N+2; ++i) {
        for (int j = 0; j < t->N+2; ++j) {
            char n[256];
            sprintf(n, "y_%d,%d", i, j);
            y[i][j].setName(n);
        }
            model.add(y[i]);
    }
#ifdef __show_steps_subp
    printf("Added y_(i,j) vars\n");
#endif

    // Adds the vars for the vertex
    IloNumVarArray v = IloNumVarArray(env, t->N+2, 0, 1);
    for (int i = 0; i < t->N; ++i) {
        char n[256];
        sprintf(n, "v_%d", i);
        v[i].setName(n);
    }
    model.add(v);
#ifdef __show_steps_subp
    printf("Added v_a vars\n");
#endif

    { //Sets the objective function
        IloNumExpr obj(env);
        for (int i = 0; i < t->N+2; ++i) {
            for (int j = 0; j < t->N+2; ++j) {
                obj += cost_mat[i][j] * y[i][j];
            }
        }

        for (int i = 0; i < t->N; ++i) {
            obj -= duals[i] * v[i];
        }

        obj -= duals[t->N]; // Mi
        model.add(IloMinimize(env, obj));
        obj.end();
    }


    // Source and Sink contraints
    { IloNumExpr expr(env); // Source
        for (int i = 0; i < t->N; ++i) {
            expr += adj_mat[i][t->N+1] * y[i][t->N+1];
        }
        model.add(expr == 1);
        expr.end();
    } {
        IloNumExpr expr(env); // Sink
        for (int i = 0; i < t->N; ++i) {
            expr += adj_mat[t->N][i] * y[t->N][i];
        }
        model.add(expr == 1);
        expr.end();
    }
#ifdef __show_steps_subp
    printf("Added Sink and Source constraints\n");
#endif

    // Flow conservation contraints
    // TODO: Double check this later
    for (int j = 0; j < t->N; ++j) {
        {
            IloNumExpr expr(env);
            for (int i = 0; i < t->N+2; ++i) {
                expr += adj_mat[i][j] * y[i][j];
            }
            model.add(expr == v[j]);
            expr.end();
        } {
            IloNumExpr expr(env);
            for (int k = 0; k < t->N+2; ++k) {
                expr += adj_mat[j][k] * y[j][k];
            }
            model.add(expr == v[j]);
            expr.end();
        }
    }
#ifdef __show_steps_subp
    printf("Added flow conservation constraints\n");
#endif

    { // Time limite contraint
        IloNumExpr expr(env);
        for (int j = 0; j < t->N+2; ++j) {
            for (int i = 0; i < t->N+2; ++i) {
                expr += time_mat[i][j] * y[i][j];
            }
        }
        model.add(expr <= t->time_limit);
        expr.end();
    }
#ifdef __show_steps_subp
    printf("Added time limit contraint\n");
#endif

    if ( !cplex.solve() ) {
        env.error() << "Failed to optimize SubProblem" << endl;
        throw(-1);
    }

    IloNumArray vals(env);
#ifdef __show_steps_subp
    env.out() << "Solution status = " << cplex.getStatus() << endl;
    env.out() << "Solution value  = " << cplex.getObjValue() << endl;
#endif
    for (int i = 0; i < t->N+2; ++i) {
        cplex.getValues(vals, y[i]);
#ifdef __show_steps_subp
        env.out() << "y["<<i<<"]      = " << vals << endl;
#endif
        }
    cplex.getValues(vals, v);
#ifdef __show_steps_subp
    env.out() << "v         = " << vals << endl;
#endif

    cplex.exportModel("subp.lp");
}
