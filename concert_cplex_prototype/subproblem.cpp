#include <iostream>
#include <stdio.h>

#include <vector>
#include <map>

#include "types.h"
#include "utils.h"
#include "greedy_heur.h"
#include "exact_subproblem.h"
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
    bool runGreedyHeurLp          = false;
    bool runGreedyHeur            = false;
    bool runSimmulatedAnnealing   = false;
    bool runAntColonyOptimization = false;
    bool runExact                 = true;

    double objValue               = 0;

    _journey journey;
    init_journey(journey);

    // greedyLpHeur
    if ( runGreedyHeurLp ) {
        //journey = greedyLpHeur ( csp, sp, y, env, cplex_final, &objValue );

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
    if ( runGreedyHeur ) {
        journey = greedyHillClimbingHeur ( csp, sp, &objValue );

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
    if ( runSimmulatedAnnealing ) {
        journey = simmulatedAnnealing ( csp, sp, &objValue );

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
    if ( runAntColonyOptimization ) {
        journey = antColonyOptmization ( csp, sp, &objValue );

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

    // Exact solution
    if ( runExact ) {
        journey = subproblemExactSolve(csp, sp, &objValue) ;
        //printf("exact solution is : %4.3f\n", objValue);

        if ( objValue < 0 ) {
            if ( sp->usedJourneys.count(journey.covered) == 0 ) {
                printf("exact solution is good\n");
            } else {
                printf("exact solution not unique\n");
            }
        } else {
            printf("exact solution is bad\n");
        }

        *reduced_cost = objValue;
        return journey;
    }
    //End of Exact solution

    *reduced_cost = 1; // This is enough to signal the master problem that no good journey was found
    // This code should be actually unreachable, since the exact solved always will return before
    return journey;
}
