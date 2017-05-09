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
#include "load_balancer.h"
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
    //bool runGreedyHeurLp          = false;
    bool runGreedyHeur            = false;
    bool runSimmulatedAnnealing   = false;
    bool runAntColonyOptimization = false;
    bool runTabuSearch            = false;
    bool runExact                 = true;

    runGreedyHeur            = true;
    runSimmulatedAnnealing   = true;
    runAntColonyOptimization = true;
    runTabuSearch            = true;

    std::vector<bool> usable;

    int nruns = 0;

    if ( runGreedyHeur ) {
        usable.push_back(true);
        nruns ++;
    } else {
        usable.push_back(false);
    }

    if ( runSimmulatedAnnealing ) {
        usable.push_back(true);
        nruns ++;
    } else {
        usable.push_back(false);
    }

    if ( runAntColonyOptimization ) {
        usable.push_back(true);
        nruns ++;
    } else {
        usable.push_back(false);
    }

    if ( runTabuSearch ) {
        usable.push_back(true);
        nruns ++;
    } else {
        usable.push_back(false);
    }

    lb_ctrl_init();
    lb_ctrl_reset_used();
    lb_ctrl_set_usable(usable);

    double objValue               = 0;

    _perf_data *perf_data = get_perf_pointer();

    _journey journey;
    init_journey(journey);

    struct timespec t1;
    struct timespec t2;

    for (int i = 0; i < nruns; ++i) {
        int choice = lb_ctrl_get_next();
        switch (choice) {
            case 0:
                // greedyHeur
                {
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
                break;

            case 1:
                // Simmulated Annealing
                {
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
                break;

            case 2:
                // Ant Colony Optimization
                {
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
                // End of Ant Colony Optimization
                break;

            case 3:
                // TabuSearch
                {
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
                break;

            default:
                fprintf(stderr, "PANIC! Got an invalid heuristic. Aborting!\n");
                exit(-1);
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
