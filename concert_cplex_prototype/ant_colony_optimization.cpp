#include <float.h>
#include <math.h>

#include "types.h"
#include "ant_colony_optimization.h"
#include "utils.h"

double min(double a, double b) {
    return a < b ? a : b;
}

double costACO ( _graph &graph ) {
    return 0;
}

_journey ACOdoAntWalk ( _graph &graph, double c_heur, double c_greed ) {
    _journey candidate;

    int atual = graph.size() - 2; // It starts at the first virtual node (V_0)

    do {
        double prob = drand48();

        if ( prob <= c_greed ) { // Greedy choice, grabs the next task with the best (lowest) reduced cost
            int bestIndex = 0;
            double bestCost = FLT_MAX;

            for (int i = 0; i < (int)graph[atual].size(); ++i) {
                double w = graph[atual][i].cost;
                if ( w < bestCost ) {
                    bestIndex = i;
                    bestCost = w;
                }
            }

            candidate.cost += graph[atual][bestIndex].cost;
            candidate.time += graph[atual][bestIndex].time;
            candidate.covered.push_back(bestIndex);

            atual = bestIndex;
        } else { // This is the heuristic choice

            // this first step calculates all the probabilities
            double c_hist = 1.0;
            double sum    = 0;

            for (int i = 0; i < (int)graph[atual].size(); ++i) {
                graph[atual][i].history   = pow( graph[atual][i].pheromone, c_hist);
                graph[atual][i].heuristic = pow( 1.0 / graph[atual][i].scaled_cost, c_heur);
                graph[atual][i].prob      = graph[atual][i].history * graph[atual][i].heuristic;

                sum += graph[atual][i].prob;
            }

            // Now this step uses a roulette to randomly pick a journey favoring the best ones
            // using c_heur as a control for how much it tends towards the best one

            double v = drand48();

            int step = 0;

            do {
                v -= graph[atual][step].prob / sum;

                step ++;
            } while ( v > 0 && step < (int) graph[atual].size() );

        }
    } while ( atual < (int)graph.size() - 2 );

    return candidate;
}

_journey antColonyOptmization ( _csp *csp, _subproblem_info *sp, double *objValue ) {
    int    max_it        = 100;
    int    num_ants      =  10;
    double decay         = 0.1;
    double c_heur        = 2.5;
    double c_local_phero = 0.1;
    double c_greed       = 0.9;

    double init_pheromone;

    _graph graph;

    graph.resize(csp->N + 2); // This has the virtual starting and ending nodes

    double minCost = FLT_MAX;

    // Copies the original graph to a graph with the info necessary to run ACO
    for (int i = 0; i < (int) csp->graph.size(); ++i) {
        for (int j = 0; j < (int) csp->graph[i].size(); ++j) {
            _path p;

            p.dest           = csp->graph[i][j].dest;
            p.cost           = sp->cost_mat[i][p.dest];
            p.time           = sp->time_mat[i][p.dest];
            p.dual_cost      = sp->duals[p.dest];
            p.scaled_cost    = p.cost - sp->duals[p.dest];
            p.effective_cost = p.cost - sp->duals[p.dest];
            p.pheromone      = 0.0;

            graph[i].push_back(p);

            minCost = min(p.scaled_cost, minCost);

            //printf(" %3d -> %3d cost %5.1f time %5.1f dual %5.1f redcost %7.1f minCost %5.1f\n", i, p.dest, p.cost, p.time, p.dual_cost, p.scaled_cost, minCost);
        }

        // This adds the sink final node
        _path p;

        p.dest           = csp->N + 1;
        p.cost           = sp->cost_mat[i][p.dest];
        p.time           = sp->time_mat[i][p.dest];
        p.dual_cost      = sp->duals[p.dest];
        p.scaled_cost    = p.cost - sp->duals[p.dest];
        p.effective_cost = p.cost - sp->duals[p.dest];
        p.pheromone      = 0.0;

        graph[i].push_back(p);

        minCost = min(p.scaled_cost, minCost);
    }

    // This leads the first node that leads ato all others
    for (int i = 0; i < csp->N; ++i) {
        _path p;

        p.dest           = i;
        p.cost           = sp->cost_mat[i][p.dest];
        p.time           = sp->time_mat[i][p.dest];
        p.dual_cost      = sp->duals[p.dest];
        p.scaled_cost    = p.cost - sp->duals[p.dest];
        p.effective_cost = p.cost - sp->duals[p.dest];
        p.pheromone      = 0.0;

        graph[csp->N].push_back(p);

        minCost = min(p.scaled_cost, minCost);
    }

    if ( minCost < 0 ) { // If minCost is zero or more no useful solution can be found
        double total = 0;
        for (int i = 0; i < (int) graph.size(); ++i) {
            for (int j = 0; j < (int) graph[i].size(); ++j) {
                graph[i][j].scaled_cost += -minCost + 1; // This will make everything at least one
                total += graph[i][j].scaled_cost;
                printf("%3d -> %3d   %7.1f\n", i, graph[i][j].dest, graph[i][j].scaled_cost);
            }
        }

        init_pheromone = 1.0 / (total * csp->N);

        for (int i = 0; i < (int) graph.size(); ++i) {
            for (int j = 0; j < (int) graph[i].size(); ++j) {
                // FIXME
                // Might change this later. It is kinda hackish,
                // should be an actual solution cost for total
                graph[i][j].pheromone = init_pheromone;
            }
        }
    } else {
        *objValue = 1;
        _journey journey;
        init_journey(journey);
        return journey;
    }

    for (int iter = 0; iter < max_it; ++iter) {
        for (int antIndex = 0; antIndex < num_ants; ++antIndex) {
            _journey candidate = ACOdoAntWalk ( graph, c_heur, c_greed );

            //updateLocalPheromones
        }
        //updateGlobalPheromones
    }

    exit(0);

    _journey solution;
    return solution;
}
