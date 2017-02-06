#include <iostream>
#include <stdio.h>

#include <vector>
#include <map>

#include "types.h"
#include "greedy_heur.h"
#include "simmulated_annealing.h"
#include "ant_colony_optimization.h"

#include <ilcplex/ilocplex.h>
#include <ilcp/cpext.h>

//#define __show_steps_subp

using namespace std;

void update_subproblem_duals ( _subproblem_info *sp, _csp *csp, IloNumArray duals ) {
    printf("SUBPROBLEM UPDATE DUALS\n");
    for (int i = 0; i < csp->N; ++i) {
        sp->duals[i] = duals[i];
    }

    sp->mi = duals[csp->N];
}

void init_subproblem_info ( _subproblem_info *sp, _csp *csp ) {
    printf("INIT SUBPROBLEM INFO\n");

    sp->adj_mat  = (double**) malloc ( sizeof ( double* ) * (csp->N+2) );
    sp->time_mat = (double**) malloc ( sizeof ( double* ) * (csp->N+2) );
    sp->cost_mat = (double**) malloc ( sizeof ( double* ) * (csp->N+2) );
    sp->duals    = (double* ) malloc ( sizeof ( double  ) * (csp->N  ) );

    sp->N        = csp->N;

    if ( sp->adj_mat == NULL || sp->time_mat == NULL || sp->cost_mat == NULL || sp->duals == NULL ) {
        printf("adj_mat = %p\ntime_mat = %p\ncost_mat = %p\nsp->duals %p\n", sp->adj_mat , sp->time_mat, sp->cost_mat, sp->duals);
        exit(-1);
    }

    for (int i = 0; i < csp->N+2; ++i) {
        sp->adj_mat [i] = (double*) malloc ( sizeof (double) * (csp->N+2) );
        sp->time_mat[i] = (double*) malloc ( sizeof (double) * (csp->N+2) );
        sp->cost_mat[i] = (double*) malloc ( sizeof (double) * (csp->N+2) );

        if ( sp->adj_mat[i] == NULL || sp->time_mat[i] == NULL || sp->cost_mat[i] == NULL ) {
            printf("adj_mat = %p\ntime_mat = %p\n cost_mat = %p", sp->adj_mat[i] , sp->time_mat[i], sp->cost_mat[i]);
            exit(-1);
        }
    }

    for (int i = 0; i < csp->N+2; ++i) {
        for (int j = 0; j < csp->N+2; ++j) {
            sp->adj_mat [i][j] = 0;
            sp->cost_mat[i][j] = 0;
            sp->time_mat[i][j] = 0;
        }
    }

    // This populates an adjacency matrix, cost matrix and a time matrix
    for (int i = 0; i < (int)csp->graph.size(); ++i) {
        for (int j = 0; j < (int)csp->graph[i].size(); ++j) {
            sp->adj_mat [i][csp->graph[i][j].dest] = 1.0;
            sp->cost_mat[i][csp->graph[i][j].dest] = csp->graph[i][j].cost;
            sp->time_mat[i][csp->graph[i][j].dest] = csp->task[csp->graph[i][j].dest].end_time -
                                                     csp->task[i].end_time;
        }
    }

    // Adds the 2 virtual nodes
    for (int i = 0; i < csp->N; ++i) {
        sp->adj_mat [csp->N][i       ] = 1.0;
        sp->adj_mat [i     ][csp->N+1] = 1.0; // This is the starting node

        sp->cost_mat[csp->N][i       ] = 0.0;
        sp->cost_mat[i     ][csp->N+1] = 0.0;

        sp->time_mat[csp->N][i       ] = csp->task[i].end_time - csp->task[i].start_time; // This accounts the time for the first task in a journey
        sp->time_mat[i     ][csp->N+1] = 0.0;
    }
}

_journey subproblem(IloNumArray duals, _csp *csp, _subproblem_info *sp, double *reduced_cost) {
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
            obj -= duals[i] * v[i];
        }

        obj -= duals[csp->N]; // Mi
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

    // // Beginning of the HEURISTIC section

    // Copies the model to solve the linear relaxixation
    IloModel model_final(env);
    IloCplex cplex_final(model_final);
    model_final.add(model);
    model_final.add(IloConversion(env, v, ILOFLOAT));

    for (int i = 0; i < csp->N+2; ++i) {
        model_final.add(IloConversion(env, y[i], ILOFLOAT));
    }

    cplex_final.setOut(env.getNullStream());
    if ( !cplex_final.solve() ) {
        env.error() << "Failed to optimize Relaxed subproblem" << endl;
        throw(-1);
    }

    // Extracts the values
    IloNumArray vals(env);
    env.out() << "Solution status = " << cplex_final.getStatus() << endl;
    env.out() << "Solution value  = " << cplex_final.getObjValue() << endl;

    // greedyLpHeur
    if ( 0 ) {
        double objValue = 0;

        _journey journey = greedyLpHeur ( csp, sp, y, env, cplex_final, &objValue );

        if ( objValue < 0 ) {
            if ( sp->usedJourneys.count(journey.covered) == 0 ) {
                printf("greedyLpHeuristic solution is good\n");
                *reduced_cost = objValue;
                return journey;
            } else {
                printf("greedyLpHeuristic solution not unique\n");
            }
        } else {
            printf("greedyLpHeuristic solution is bad\n");
            //exit(0);
            //Do Nothing
        }
    }
    //End of greedyLpHeur

    // greedyHeur
    if ( 0 ) {
        double objValue = 0;

        _journey journey = greedyHillClimbingHeur ( csp, sp, &objValue );

        if ( objValue < 0 ) {
            if ( sp->usedJourneys.count(journey.covered) == 0 ) {
                printf("greedyHillClimbingHeur solution is good\n");
                *reduced_cost = objValue;
                return journey;
            } else {
                printf("greedyHillClimbingHeur solution not unique\n");
            }
        } else {
            printf("greedyHillClimbingHeur solution is bad\n");
            //exit(0);
            //Do Nothing
        }
    }
    //End of greedyHeur

    // Simmulated Annealing
    if ( 0 ) {
        double objValue = 0;

        _journey journey = simmulatedAnnealing ( csp, sp, &objValue );

        if ( objValue < 0 ) {
            if ( sp->usedJourneys.count(journey.covered) == 0 ) {
                printf("simmulatedAnnealing solution is good\n");
                *reduced_cost = objValue;
                return journey;
            } else {
                printf("simmulatedAnnealing solution not unique\n");
            }
        } else {
            printf("simmulatedAnnealing solution is bad\n");
            //exit(0);
            //Do Nothing
        }
    }
    //End of Simmulated Annealing

    // Ant Colony Optimization
    if ( 0 ) {
        double objValue = 0;

        _journey journey = antColonyOptmization ( csp, sp, &objValue );

        if ( objValue < 0 ) {
            if ( sp->usedJourneys.count(journey.covered) == 0 ) {
                printf("antColonyOptmization solution is good\n");
                *reduced_cost = objValue;
                return journey;
            } else {
                printf("antColonyOptmization solution not unique\n");
            }
        } else {
            printf("antColonyOptmization solution is bad\n");
            //exit(0);
            //Do Nothing
        }
    }
    //End of Ant Colony Optimization

    if ( !cplex.solve() ) {
        env.error() << "Failed to optimize SubProblem" << endl;
        throw(-1);
    }

    // Reads the solution from the model and stores in a struct to return to the master problem
    _journey journey;

    journey.time = 0;
    journey.cost = 0;

    //IloNumArray vals(env);
    //env.out() << "Solution status = " << cplex.getStatus() << endl;
    //env.out() << "Solution value  = " << cplex.getObjValue() << endl;
    double x = cplex.getObjValue();
    *reduced_cost = x;
    for (int i = 0; i < csp->N+2; ++i) {
        cplex.getValues(vals, y[i]);
        //env.out() << "y["<<i<<"]      = " << vals << endl;
        for (int j = 0; j < csp->N+2; ++j) {
            if ( vals[j] > 0.5 ) {
                printf("y[%2d, %2d] = %2.8f, cost = %2.4f, time = %2.4f\n", i, j, vals[j], sp->cost_mat[i][j], sp->time_mat[i][j]);
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
            printf("v[%2d] = %2.4f\n", i, vals[i]);
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

    //cplex.exportModel("subp.lp");
    //exit(0);

    return journey;
}
