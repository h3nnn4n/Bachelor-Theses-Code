#include <iostream>
#include <stdio.h>

#include <vector>
#include <map>

#include "types.h"

#include <ilcplex/ilocplex.h>
#include <ilcp/cpext.h>

//#define __show_steps_subp

using namespace std;

_journey subproblem(IloNumArray reduced_costs, IloNumArray duals, _csp *t, std::vector<_journey> &journeys, double *reduced_cost, std::map<std::vector<int>, bool> &usedJourneys) {
    double adj_mat [t->N + 2][t->N + 2];
    double time_mat[t->N + 2][t->N + 2];
    double cost_mat[t->N + 2][t->N + 2];

    IloEnv         env         ;
    IloModel       model(env  );
    IloCplex       cplex(model);
    cplex.setOut(env.getNullStream());

    for (int i = 0; i < t->N+2; ++i) {
        for (int j = 0; j < t->N+2; ++j) {
            adj_mat [i][j] = 0;
            cost_mat[i][j] = 0;
            time_mat[i][j] = 0;
        }
    }

    // This populates an adjacency matrix, cost matrix and a time matrix
    //printf("%d\n", (int)t->graph.size());
    for (int i = 0; i < (int)t->graph.size(); ++i) {
        for (int j = 0; j < (int)t->graph[i].size(); ++j) {
            //printf("i = %2d j = %2d dest = %2d", i, j, t->graph[i][j].dest);
            adj_mat [i][t->graph[i][j].dest] = 1.0;
            cost_mat[i][t->graph[i][j].dest] = t->graph[i][j].cost;
            //time_mat[i][t->graph[i][j].dest] = (t->task[t->graph[i][j].dest].end_time    -
                                                //t->task[t->graph[i][j].dest].start_time) +
                                               //(t->task[i].end_time                      -
                                                //t->task[i].start_time                  ) ;
            //printf(" -> ");
            //time_mat[i][t->graph[i][j].dest] = (t->task[i].end_time    -
                                                //t->task[t->graph[i][j].dest].start_time) +
                                               //(t->task[t->graph[i][j].dest].end_time    -
                                                //t->task[t->graph[i][j].dest].start_time  ) ;
            time_mat[i][t->graph[i][j].dest] = t->task[t->graph[i][j].dest].end_time    -
                                               t->task[i].end_time;

            //printf("i = (%2d, %2d) j = (%2d, %2d)",
              //t->task[i].start_time,
              //t->task[i].end_time,
              //t->task[t->graph[i][j].dest].start_time,
              //t->task[t->graph[i][j].dest].end_time);

            //printf("\n");
        }
    }

    //exit(0);

    // Adds the 2 virtual nodes
    for (int i = 0; i < t->N; ++i) {
        adj_mat [t->N][i     ] = 1.0;
        adj_mat [i   ][t->N+1] = 1.0; // This is the starting node

        cost_mat[t->N][i     ] = 0.0;
        cost_mat[i   ][t->N+1] = 0.0;

        //time_mat[t->N][i     ] = 0.0;
        //time_mat[i   ][t->N+1] = t->task[i].end_time - t->task[i].start_time;

        time_mat[t->N][i     ] = t->task[i].end_time - t->task[i].start_time; // This accounts the time for the first task in a journey
        time_mat[i   ][t->N+1] = 0.0;
    }

    // Print the matrix, for debugging purposes
    //for (int i = 0; i < t->N+2; ++i) {
        //for (int j = 0; j < t->N+2; ++j) {
            //printf("%2.1f ", adj_mat[i][j]);
            ////printf("%2.1f ", time_mat[i][j]);
            ////printf("%2.1f ", cost_mat[i][j]);
        //}
        //printf("\n");
    //}

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
    //printf("Added y_(i,j) vars\n");

    // Adds the vars for the vertex
    IloNumVarArray v = IloNumVarArray(env, t->N+2, 0, 1, ILOINT);
    for (int i = 0; i < t->N; ++i) {
        char n[256];
        sprintf(n, "v_%d", i);
        v[i].setName(n);
    }
    model.add(v);
    //printf("Added v_a vars\n");

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
    //printf("Added Sink and Source constraints\n");

    // Flow conservation contraints
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
    //printf("Added flow conservation constraints\n");

    { // Time limite contraint
        IloNumExpr expr(env);
        for (int j = 0; j < t->N+2; ++j) {
            for (int i = 0; i < t->N+2; ++i) {
                expr += time_mat[i][j] * y[i][j];
            }
        }
        model.add(expr <= t->time_limit);
        EXPR.end();
    }
    //printf("Added time limit contraint\n");
    // // Beginning of the HEURISTIC section
    //
    // Copies the model to solve the linear relaxixation
    IloModel model_final(env);
    IloCplex cplex_final(model_final);
    model_final.add(model);
    model_final.add(IloConversion(env, v, ILOFLOAT));

    for (int i = 0; i < t->N+2; ++i) {
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
    {
        //// Copies the model to solve the linear relaxixation
        //IloModel model_final(env);
        //IloCplex cplex_final(model_final);
        //model_final.add(model);
        //model_final.add(IloConversion(env, v, ILOFLOAT));

        //for (int i = 0; i < t->N+2; ++i) {
            //model_final.add(IloConversion(env, y[i], ILOFLOAT));
        //}

        //if ( !cplex_final.solve() ) {
            //env.error() << "Failed to optimize Relaxed subproblem" << endl;
            //throw(-1);
        //}

        //// Extracts the values
        //IloNumArray vals(env);
        //env.out() << "Solution status = " << cplex_final.getStatus() << endl;
        //env.out() << "Solution value  = " << cplex_final.getObjValue() << endl;

        //double x = cplex_final.getObjValue();
        //*reduced_cost = x;
        for (int i = 0; i < t->N+2; ++i) {
            cplex_final.getValues(vals, y[i]);
            //env.out() << "y["<<i<<"]      = " << vals << endl;
            for (int j = 0; j < t->N+2; ++j) {
                if ( vals[j] > 0.0 ) {
                    printf("y[%2d, %2d] = %2.16f, cost = %4.4f, time = %4.4f\n", i, j, vals[j], cost_mat[i][j], time_mat[i][j]);
                    //journey.cost += cost_mat[i][j];
                    //journey.time += time_mat[i][j];
                }
            }
        }

        // This reads the covered vertices
        cplex_final.getValues(vals, v);
        //env.out() << "v         = " << vals << endl;
        for (int i = 0; i < t->N; ++i) {
            if ( vals[i] > 0.0 ) {
                //journey.covered.push_back(i);
                printf("v[%2d] = %2.4f\n", i, vals[i]);
            }
        }

        _journey journey;
        journey.time = 0;
        journey.cost = 0;
        journey.covered.clear();

        double best = 0;
        double objValue = 0;
        int bestIndex = -1;
        int doit = 1;
        int atual = t->N;

        do {
            cplex_final.getValues(vals, y[atual]);

            for (int i = 0; i < t->N+2; ++i) {
                if ( vals[i] > best ) {
                    bestIndex = i;
                    best = vals[i];
                }
            }

            if ( bestIndex != -1 && best > 0 && bestIndex <= t->N ) {
                int j = bestIndex;

                if ( journey.time + time_mat[atual][j] < t->time_limit ) {
                    journey.cost += cost_mat[atual][j];
                    journey.time += time_mat[atual][j];

                    objValue += cost_mat[atual][j];

                    //printf("Covering %3d -> %3d = %4.8f  cost = %4.4f time = %4.4f dual = %4.4f\n", atual, j, vals[j], (float)cost_mat[atual][j], (float)time_mat[atual][j], duals[j] );
                    printf("Covering %3d -> %3d", atual, j);
                    printf(" = %4.8f  cost = %4.4f time = %4.4f dual = %4.4f\n", vals[j], (float)cost_mat[atual][j], (float)time_mat[atual][j], duals[j] );

                    atual = bestIndex;
                    best  = 0;

                    objValue -= duals[atual];

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
        } while ( doit && atual < t->N+1 );
        printf("Reduced value = %4.8f\n", objValue);

        if ( objValue < 0 ) {
            if ( usedJourneys.count(journey.covered) == 0 ) {
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
    if ( 0 )
    {
        // Copies the model to solve the linear relaxixation
        //IloModel model_final(env);
        //IloCplex cplex_final(model_final);
        //model_final.add(model);
        //model_final.add(IloConversion(env, v, ILOFLOAT));

        //for (int i = 0; i < t->N+2; ++i) {
            //model_final.add(IloConversion(env, y[i], ILOFLOAT));
        //}

        //if ( !cplex_final.solve() ) {
            //env.error() << "Failed to optimize Relaxed subproblem" << endl;
            //throw(-1);
        //}

        // Extracts the values
        //IloNumArray vals(env);
        //env.out() << "Solution status = " << cplex_final.getStatus() << endl;
        //env.out() << "Solution value  = " << cplex_final.getObjValue() << endl;

        //double x = cplex_final.getObjValue();
        //*reduced_cost = x;
        for (int i = 0; i < t->N+2; ++i) {
            cplex_final.getValues(vals, y[i]);
            //env.out() << "y["<<i<<"]      = " << vals << endl;
            for (int j = 0; j < t->N+2; ++j) {
                if ( vals[j] > 0.0 ) {
                    printf("y[%2d, %2d] = %2.16f, cost = %4.4f, time = %4.4f\n", i, j, vals[j], cost_mat[i][j], time_mat[i][j]);
                    //journey.cost += cost_mat[i][j];
                    //journey.time += time_mat[i][j];
                }
            }
        }

        // This reads the covered vertices
        cplex_final.getValues(vals, v);
        //env.out() << "v         = " << vals << endl;
        for (int i = 0; i < t->N; ++i) {
            if ( vals[i] > 0.0 ) {
                //journey.covered.push_back(i);
                printf("v[%2d] = %2.4f\n", i, vals[i]);
            }
        }

        _journey journey;
        journey.time = 0;
        journey.cost = 0;
        journey.covered.clear();

        double best = 0;
        double objValue = 0;
        int bestIndex = -1;
        int doit = 1;
        int atual = t->N;

        do {
            cplex_final.getValues(vals, y[atual]);

            for (int i = 0; i < t->N+2; ++i) {
                if ( vals[i] > best ) {
                    bestIndex = i;
                    best = vals[i];
                }
            }

            if ( bestIndex != -1 && best > 0 && bestIndex <= t->N ) {
                int j = bestIndex;

                if ( journey.time + time_mat[atual][j] < t->time_limit ) {
                    journey.cost += cost_mat[atual][j];
                    journey.time += time_mat[atual][j];

                    objValue += cost_mat[atual][j];

                    //printf("Covering %3d -> %3d = %4.8f  cost = %4.4f time = %4.4f dual = %4.4f\n", atual, j, vals[j], (float)cost_mat[atual][j], (float)time_mat[atual][j], duals[j] );
                    printf("Covering %3d -> %3d", atual, j);
                    printf(" = %4.8f  cost = %4.4f time = %4.4f dual = %4.4f\n", vals[j], (float)cost_mat[atual][j], (float)time_mat[atual][j], duals[j] );

                    atual = bestIndex;
                    best  = 0;

                    objValue -= duals[atual];

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
        } while ( doit && atual < t->N+1 );
        printf("Reduced value = %4.8f\n", objValue);

        if ( objValue < 0 ) {
            if ( usedJourneys.count(journey.covered) == 0 ) {
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
    for (int i = 0; i < t->N+2; ++i) {
        cplex.getValues(vals, y[i]);
        //env.out() << "y["<<i<<"]      = " << vals << endl;
        for (int j = 0; j < t->N+2; ++j) {
            if ( vals[j] > 0.5 ) {
                printf("y[%2d, %2d] = %2.8f, cost = %2.4f, time = %2.4f\n", i, j, vals[j], cost_mat[i][j], time_mat[i][j]);
                journey.cost += cost_mat[i][j];
                journey.time += time_mat[i][j];
            }
        }
    }

    // This reads the covered vertexes
    cplex.getValues(vals, v);
    //env.out() << "v         = " << vals << endl;
    for (int i = 0; i < t->N; ++i) {
        if ( vals[i] > 0.5 ) {
            journey.covered.push_back(i);
            printf("v[%2d] = %2.4f\n", i, vals[i]);
        }
    }

    // Rountine to ensure that the time_limit constraints arent violated due to numerical precision errors
    if ( journey.time > t->time_limit ) {
        printf("Journey too long!\n");
        printf("-------------------\n");
        for (int i = 0; i < t->N+2; ++i) {
            cplex.getValues(vals, y[i]);
            //env.out() << "y["<<i<<"]      = " << vals << endl;
            for (int j = 0; j < t->N+2; ++j) {
                if ( vals[j] > 10e-4 ) {
                    printf("y[%2d, %2d] = %2.18f, cost = %2.4f, time = %2.4f\n", i, j, vals[j], cost_mat[i][j], time_mat[i][j]);
                    journey.cost += cost_mat[i][j];
                    journey.time += time_mat[i][j];
                }
            }
        }
    }

    assert ( journey.time <= t->time_limit && "Journey too long!");

    //cplex.exportModel("subp.lp");
    //exit(0);

    return journey;
}
