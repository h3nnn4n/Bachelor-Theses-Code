#include <iostream>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <vector>
#include <map>

#include "types.h"
#include "utils.h"
#include "perf_data.h"
#include "time_keeper.h"
#include "greedy_heur.h"
#include "tabu_search.h"
#include "exact_subproblem.h"
#include "simmulated_annealing.h"
#include "ant_colony_optimization.h"

#include <ilcplex/ilocplex.h>
#include <ilcp/cpext.h>

//#define __show_steps_subp

using namespace std;

bool output_goodbad = false;

void update_subproblem_duals ( _subproblem_info *sp, _csp *csp, IloNumArray duals ) {
    //printf("SUBPROBLEM UPDATE DUALS\n");
    for (int i = 0; i < csp->N; ++i) {
        sp->duals[i] = duals[i];
    }

    sp->mi = duals[csp->N];
}

void init_subproblem_info ( _subproblem_info *sp, _csp *csp ) {
    //printf("INIT SUBPROBLEM INFO\n");

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

_journey subproblem(_csp *csp, _subproblem_info *sp, double *reduced_cost) {
    bool runGreedyHeurLp          = false;
    bool runGreedyHeur            = false;
    bool runSimmulatedAnnealing   = false;
    bool runAntColonyOptimization = false;
    bool runTabuSearch            = false;
    bool runExact                 = true;

    //runGreedyHeurLp          = true;
    runGreedyHeur            = true;
    runSimmulatedAnnealing   = true;
    runAntColonyOptimization = true;
    runTabuSearch            = true;

    double objValue               = 0;

    _perf_data *perf_data = get_perf_pointer();

    _journey journey;
    init_journey(journey);

    struct timespec t1;
    struct timespec t2;

    // greedyLpHeur
    if ( runGreedyHeurLp ) {
        //journey = greedyLpHeur ( csp, sp, y, env, cplex_final, &objValue );

        if ( objValue < 0 ) {
            if ( sp->usedJourneys.count(journey.covered) == 0 ) {
                if(output_goodbad)printf("greedyLpHeuristic solution is good\n");
                *reduced_cost = objValue;
                return journey;
            } else {
                if(output_goodbad)printf("greedyLpHeuristic solution not unique\n");
            }
        } else {
            if(output_goodbad)printf("greedyLpHeuristic solution is bad\n");
            //exit(0);
            //Do Nothing
        }
    }
    //End of greedyLpHeur

    // TabuSearch
    if ( runTabuSearch ) {
        timekeeper_tic(&t1);
        journey = tabuSearch ( csp, sp, &objValue );
        timekeeper_tic(&t2);

        perf_data->tabu.executions += 1;
        perf_data->tabu.time += time_diff_double(t1, t2);

        if ( objValue < 0 ) {
            if ( sp->usedJourneys.count(journey.covered) == 0 ) {
                if(output_goodbad)printf("tabuSearch solution is good\n");
                *reduced_cost = objValue;
                perf_data->tabu.good_executions += 1;
                perf_data->tabu.good_time += time_diff_double(t1, t2);
                return journey;
            } else {
                if(output_goodbad)printf("tabuSearch solution not unique\n");
                perf_data->tabu.not_unique_executions += 1;
                perf_data->tabu.not_unique_time += time_diff_double(t1, t2);
                //*reduced_cost = 1;
                //return journey;
            }
        } else {
            if(output_goodbad)printf("tabuSearch solution is bad\n");
            perf_data->tabu.bad_executions += 1;
            perf_data->tabu.bad_time += time_diff_double(t1, t2);
            //exit(0);
            //Do Nothing
        }
    }
    //End of TabuSearch

    // greedyHeur
    if ( runGreedyHeur ) {
        timekeeper_tic(&t1);
        journey = greedyHillClimbingHeur ( csp, sp, &objValue );
        timekeeper_tic(&t2);

        perf_data->greedyHill.executions += 1;
        perf_data->greedyHill.time += time_diff_double(t1, t2);

        if ( objValue < 0 ) {
            if ( sp->usedJourneys.count(journey.covered) == 0 ) {
                if(output_goodbad)printf("greedyHillClimbingHeur solution is good\n");
                perf_data->greedyHill.good_executions += 1;
                perf_data->greedyHill.good_time += time_diff_double(t1, t2);
                *reduced_cost = objValue;
                return journey;
            } else {
                if(output_goodbad)printf("greedyHillClimbingHeur solution not unique\n");
                perf_data->greedyHill.not_unique_executions += 1;
                perf_data->greedyHill.not_unique_time += time_diff_double(t1, t2);
                //*reduced_cost = 1;
                //return journey;
            }
        } else {
            if(output_goodbad)printf("greedyHillClimbingHeur solution is bad\n");
            perf_data->greedyHill.bad_executions += 1;
            perf_data->greedyHill.bad_time += time_diff_double(t1, t2);
            //exit(0);
            //Do Nothing
        }
    }
    //End of greedyHeur

    // Simmulated Annealing
    if ( runSimmulatedAnnealing ) {
        timekeeper_tic(&t1);
        journey = simmulatedAnnealing ( csp, sp, &objValue );
        timekeeper_tic(&t2);

        perf_data->sa.executions += 1;
        perf_data->sa.time += time_diff_double(t1, t2);

        if ( objValue < 0 ) {
            if ( sp->usedJourneys.count(journey.covered) == 0 ) {
                if(output_goodbad)printf("simmulatedAnnealing solution is good\n");
                perf_data->sa.good_executions += 1;
                perf_data->sa.good_time += time_diff_double(t1, t2);
                *reduced_cost = objValue;
                return journey;
            } else {
                if(output_goodbad)printf("simmulatedAnnealing solution not unique\n");
                perf_data->sa.not_unique_executions += 1;
                perf_data->sa.not_unique_time += time_diff_double(t1, t2);
                //*reduced_cost = 1;
                //return journey;
            }
        } else {
            if(output_goodbad)printf("simmulatedAnnealing solution is bad\n");
            perf_data->sa.bad_executions += 1;
            perf_data->sa.bad_time += time_diff_double(t1, t2);
            //exit(0);
            //Do Nothing
        }
    }
    //End of Simmulated Annealing

    // Ant Colony Optimization
    if ( runAntColonyOptimization ) {
        timekeeper_tic(&t1);
        journey = antColonyOptmization ( csp, sp, &objValue );
        timekeeper_tic(&t2);

        perf_data->aco.executions += 1;
        perf_data->aco.time += time_diff_double(t1, t2);

        if ( objValue < 0 ) {
            if ( sp->usedJourneys.count(journey.covered) == 0 ) {
                if(output_goodbad)printf("antColonyOptmization solution is good\n");
                *reduced_cost = objValue;
                perf_data->aco.good_executions += 1;
                perf_data->aco.good_time += time_diff_double(t1, t2);
                return journey;
            } else {
                if(output_goodbad)printf("antColonyOptmization solution not unique\n");
                perf_data->aco.not_unique_executions += 1;
                perf_data->aco.not_unique_time += time_diff_double(t1, t2);
                //*reduced_cost = 1;
                //return journey;
            }
        } else {
            if(output_goodbad)printf("antColonyOptmization solution is bad\n");
            perf_data->aco.bad_executions += 1;
            perf_data->aco.bad_time += time_diff_double(t1, t2);
            //exit(0);
            //Do Nothing
        }
    }

    // Exact solution
    if ( runExact ) {
        timekeeper_tic(&t1);
        journey = subproblemExactSolve(csp, sp, &objValue) ;
        timekeeper_tic(&t2);

        perf_data->exact.executions += 1;
        perf_data->exact.time += time_diff_double(t1, t2);

        if ( objValue < 0 ) {
            if ( sp->usedJourneys.count(journey.covered) == 0 ) {
                if(output_goodbad)printf("exact solution is good\n");
                perf_data->exact.good_executions += 1;
                perf_data->exact.good_time += time_diff_double(t1, t2);
            } else {
                if(output_goodbad)printf("exact solution not unique\n");
                perf_data->exact.not_unique_executions += 1;
                perf_data->exact.not_unique_time += time_diff_double(t1, t2);
                //*reduced_cost = 1;
                //return journey;
            }
        } else {
            if(output_goodbad)printf("exact solution is bad\n");
            perf_data->exact.bad_executions += 1;
            perf_data->exact.bad_time += time_diff_double(t1, t2);
        }

        *reduced_cost = objValue;
        return journey;
    }
    //End of Exact solution

    *reduced_cost = 1; // This is enough to signal the master problem that no good journey was found
    // This code should be actually unreachable, since the exact solved always will return before
    return journey;
}
