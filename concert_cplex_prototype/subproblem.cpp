#include <iostream>
#include <stdio.h>

#include <vector>
#include <map>

#include "types.h"

#include <ilcplex/ilocplex.h>
#include <ilcp/cpext.h>

//#define __show_steps_subp

using namespace std;

_journey greedyHillClimbingHeur ( _csp *csp, _subproblem_info *sp, double *objValue ) {
    printf("Running greedyHillClimbingHeur\n");
    _journey journey;

    for (int i = 0; i < csp->N; ++i) {
        int atual = i;
        double obj = - sp->mi - sp->duals[atual];

        journey.time = 0;
        journey.cost = 0;
        journey.covered.clear();
        journey.time += sp->time_mat[csp->N][atual];
        journey.covered.push_back(atual);

        assert ( journey.time > 0 && "JOURNEY TIME <= ZERO!!");

        bool found_something;
        do {
            found_something = false;
            for (int j = 0; j < (int) csp->graph[atual].size(); ++j) {
                int dest = csp->graph[atual][j].dest;
                if ( sp->cost_mat[atual][dest] + obj < 0 ) {
                    if ( sp->time_mat[atual][dest] + journey.time <= csp->time_limit ) {
                        journey.cost += sp->cost_mat[atual][dest];
                        journey.time += sp->time_mat[atual][dest];
                        journey.covered.push_back(dest);
                        found_something = true;
                        printf("%3d -> %3d\n", atual, dest);
                        atual = dest;
                    }
                }
            }
        } while ( found_something );

        if ( obj <= 0 && journey.covered.size() > 1 &&  sp->usedJourneys.count(journey.covered) == 0 ) {
            printf("Found new journey with reduced cost < 0\n");
            *objValue = obj;
            break;
        }
    }


    printf("Reduced value = %4.8f\n", *objValue);

    return journey;
}

_journey greedyLpHeur ( _csp *csp, _subproblem_info *sp, IloArray<IloNumVarArray> y, IloEnv &env, IloCplex &cplex_final, double *objValue ) {
    printf("Running greedyLpheur\n");
    _journey journey;
    journey.time = 0;
    journey.cost = 0;
    journey.covered.clear();

    IloNumArray vals(env);

    double best = 0;
    int bestIndex = -1;
    int doit = 1;
    int atual = csp->N;

    *objValue = 0;

    *objValue -= sp->mi; // Mi

    do {
        cplex_final.getValues(vals, y[atual]);

        for (int i = 0; i < csp->N+2; ++i) {
            if ( vals[i] > best ) {
                bestIndex = i;
                best = vals[i];
            }
        }

        if ( bestIndex != -1 && best > 0 && bestIndex <= csp->N ) {
            int j = bestIndex;

            if ( journey.time + sp->time_mat[atual][j] < csp->time_limit ) {
                journey.cost += sp->cost_mat[atual][j];
                journey.time += sp->time_mat[atual][j];

                *objValue += sp->cost_mat[atual][j];

                //printf("Covering %3d -> %3d = %4.8f  cost = %4.4f time = %4.4f dual = %4.4f\n", atual, j, vals[j], (float)cost_mat[atual][j], (float)sp->time_mat[atual][j], duals[j] );
                printf("Covering %3d -> %3d", atual, j);
                printf(" = %4.8f  cost = %4.4f time = %4.4f dual = %4.4f\n", vals[j], (float)sp->cost_mat[atual][j], (float)sp->time_mat[atual][j], sp->duals[j] );

                atual = bestIndex;
                best  = 0;

                *objValue -= sp->duals[atual];

                journey.covered.push_back ( atual );
            } else {
                printf("MaxTime limit %4.8f, stoping\n", (float)journey.time);
                doit = false;
            }
        } else {
            //assert ( 0 && "This should never happen" );
            printf("Found the last task with time %4.8f, stoping\n", (float)journey.time);
            doit = false;
        }
    } while ( doit && atual < csp->N+1 );

    printf("Reduced value = %4.8f\n", *objValue);

    return journey;
}

void update_subproblem_duals ( _subproblem_info *sp, _csp *csp, IloNumArray duals ) {
    printf("SUBPROBLEM UPDATAE DUALS\n");
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
                printf("Using greedyLpHeuristic solution\n");
                *reduced_cost = objValue;
                return journey;
            } else {
                printf("Solution not unique\n");
            }
        } else {
            printf("Solution is bad\n");
            //exit(0);
            //Do Nothing
        }
    }
    //End of greedyLpHeur

    // greedyHeur
    if ( 1 ) {
        double objValue = 0;

        _journey journey = greedyHillClimbingHeur ( csp, sp, &objValue );

        if ( objValue < 0 ) {
            if ( sp->usedJourneys.count(journey.covered) == 0 ) {
                printf("Using greedyHillClimbingHeur solution\n");
                *reduced_cost = objValue;
                return journey;
            } else {
                printf("Solution not unique\n");
            }
        } else {
            printf("Solution is bad\n");
            //exit(0);
            //Do Nothing
        }
    }
    //End of greedyHeur


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
