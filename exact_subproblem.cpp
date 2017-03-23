#include <iostream>
#include <stdio.h>

#include <vector>
#include <map>

#include "types.h"

#include <ilcplex/ilocplex.h>
#include <ilcp/cpext.h>

using namespace std;

bool exact_progrss = false;

_journey subproblemExactSolve(_csp *csp, _subproblem_info *sp, double *reduced_cost) {
    IloEnv         env         ;
    IloModel       model(env  );
    IloCplex       cplex(model);
    cplex.setOut(env.getNullStream());

    // Adds the variables for the edges
    // Makes a (N+2) x (N+2) matrix
    IloArray<IloNumVarArray> y(env, csp->N+2);
    for (int i = 0; i < csp->N+2; ++i) {
        y[i] = IloNumVarArray(env, csp->N+2, 0, 1);
    }

    // Set the variable names. This is mostly usefull
    // for debugging the model and reading the lp output
    for (int i = 0; i < csp->N+2; ++i) {
        for (int j = 0; j < csp->N+2; ++j) {
            char n[256];
            sprintf(n, "y_%d,%d", i, j);
            y[i][j].setName(n);
        }
        model.add(y[i]);
    }
    //printf("Added y_(i,j) vars\n");

    // Adds the vars for the vertex
    IloNumVarArray v = IloNumVarArray(env, csp->N+2, 0, 1, ILOINT);
    for (int i = 0; i < csp->N; ++i) {
        char n[256];
        sprintf(n, "v_%d", i);
        v[i].setName(n);
    }
    model.add(v);
    //printf("Added v_a vars\n");

    { //Sets the objective function
        IloNumExpr obj(env);
        for (int i = 0; i < csp->N+2; ++i) {
            for (int j = 0; j < csp->N+2; ++j) {
                obj += sp->cost_mat[i][j] * y[i][j];
            }
        }

        for (int i = 0; i < csp->N; ++i) {
            obj -= sp->duals[i] * v[i];
        }

        obj -= sp->mi; // Mi
        model.add(IloMinimize(env, obj));
        obj.end();
    }

    // Source and Sink contraints
    { IloNumExpr expr(env); // Source
        for (int i = 0; i < csp->N; ++i) {
            expr += sp->adj_mat[i][csp->N+1] * y[i][csp->N+1];
        }
        model.add(expr == 1);
        expr.end();
    } {
        IloNumExpr expr(env); // Sink
        for (int i = 0; i < csp->N; ++i) {
            expr += sp->adj_mat[csp->N][i] * y[csp->N][i];
        }
        model.add(expr == 1);
        expr.end();
    }
    //printf("Added Sink and Source constraints\n");

    // Flow conservation contraints
    for (int j = 0; j < csp->N; ++j) {
        {
            IloNumExpr expr(env);
            for (int i = 0; i < csp->N+2; ++i) {
                expr += sp->adj_mat[i][j] * y[i][j];
            }
            model.add(expr == v[j]);
            expr.end();
        } {
            IloNumExpr expr(env);
            for (int k = 0; k < csp->N+2; ++k) {
                expr += sp->adj_mat[j][k] * y[j][k];
            }
            model.add(expr == v[j]);
            expr.end();
        }
    }
    //printf("Added flow conservation constraints\n");

    { // Time limite contraint
        IloNumExpr expr(env);
        for (int j = 0; j < csp->N+2; ++j) {
            for (int i = 0; i < csp->N+2; ++i) {
                expr += sp->time_mat[i][j] * y[i][j];
            }
        }
        model.add(expr <= csp->time_limit);
        expr.end();
    }
    //printf("Added time limit contraint\n");

    if ( !cplex.solve() ) {
        env.error() << "Failed to optimize SubProblem" << endl;
        throw(-1);
    }

    // Reads the solution from the model and stores in a struct to return to the master problem
    _journey journey;

    journey.time = 0;
    journey.cost = 0;

    IloNumArray vals(env);
    if(exact_progrss)env.out() << "Solution status = " << cplex.getStatus() << endl;
    if(exact_progrss)env.out() << "Solution value  = " << cplex.getObjValue() << endl;
    double x = cplex.getObjValue();
    *reduced_cost = x;
    for (int i = 0; i < csp->N+2; ++i) {
        cplex.getValues(vals, y[i]);
        //env.out() << "y["<<i<<"]      = " << vals << endl;
        for (int j = 0; j < csp->N+2; ++j) {
            if ( vals[j] > 0.5 ) {
                if(exact_progrss)printf("y[%2d, %2d] = %2.8f, cost = %2.4f, time = %2.4f\n", i, j, vals[j], sp->cost_mat[i][j], sp->time_mat[i][j]);
                journey.cost += sp->cost_mat[i][j];
                journey.time += sp->time_mat[i][j];
            }
        }
    }

    // This reads the covered vertexes
    cplex.getValues(vals, v);
    //env.out() << "v         = " << vals << endl;
    for (int i = 0; i < csp->N; ++i) {
        if ( vals[i] > 0.5 ) {
            journey.covered.push_back(i);
            if(exact_progrss)printf("v[%2d] = %2.4f\n", i, vals[i]);
        }
    }

    // Rountine to ensure that the time_limit constraints arent violated due to numerical precision errors
    if ( journey.time > csp->time_limit ) {
        printf("Journey too long!\n");
        printf("-------------------\n");
        for (int i = 0; i < csp->N+2; ++i) {
            cplex.getValues(vals, y[i]);
            //env.out() << "y["<<i<<"]      = " << vals << endl;
            for (int j = 0; j < csp->N+2; ++j) {
                if ( vals[j] > 10e-4 ) {
                    printf("y[%2d, %2d] = %2.18f, cost = %2.4f, time = %2.4f\n", i, j, vals[j], sp->cost_mat[i][j], sp->time_mat[i][j]);
                    journey.cost += sp->cost_mat[i][j];
                    journey.time += sp->time_mat[i][j];
                }
            }
        }
    }

    assert ( journey.time <= csp->time_limit && "Journey too long!");

    //cplex.end();

    for (int i = 0; i < csp->N+2; ++i) {
        y[i].end();
        v[i].end();
    }

    y.end();
    v.end();

    vals.end();

    //model.end();
    env.end();

    //cplex.exportModel("subp.lp");
    //exit(0);

    return journey;
}
