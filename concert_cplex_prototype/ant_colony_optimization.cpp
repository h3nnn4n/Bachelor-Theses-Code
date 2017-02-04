#include <float.h>
#include <limits.h>
#include <math.h>
#include <assert.h>

#include "types.h"
#include "ant_colony_optimization.h"
#include "utils.h"

_subproblem_info *_sp;

static int getIndexForEdge( _graph &graph, int a, int b) {
    for (int i = 0; i < (int) graph[a].size(); ++i) {
        if ( graph[a][i].dest == b ) {
            return i;
        }
    }
    return -1;
}

bool debug_aco = false;

double min(double a, double b) {
    return a < b ? a : b;
}

double costACO ( _graph &graph ) {
    return 0;
}

double getEffectiveCost ( _graph &graph, _journey &journey) {
    std::vector<int> covered;
    covered.push_back(graph.size() - 2);

    double journey_cost = 0;

    for (int i = 0; i < (int)journey.covered.size(); ++i) {
        covered.push_back(journey.covered[i]);
    }

    for (int sourceIndex = 0; sourceIndex < (int)covered.size() - 1; ++sourceIndex) {
        int source = covered[sourceIndex    ];
        int dest   = covered[sourceIndex + 1];

        int destIndex = getIndexForEdge(graph, source, dest);
        journey_cost += graph[source][destIndex].effective_cost;
    }

    return journey_cost;
}

double getScaledCost ( _graph &graph, _journey &journey) {
    std::vector<int> covered;
    covered.push_back(graph.size() - 2);

    double journey_cost = 0;

    for (int i = 0; i < (int)journey.covered.size(); ++i) {
        covered.push_back(journey.covered[i]);
    }

    for (int sourceIndex = 0; sourceIndex < (int)covered.size() - 1; ++sourceIndex) {
        int source = covered[sourceIndex    ];
        int dest   = covered[sourceIndex + 1];

        int destIndex = getIndexForEdge(graph, source, dest);
        journey_cost += graph[source][destIndex].scaled_cost;
    }

    return journey_cost;
}

void updateGlobalPheromones( _graph &graph, _journey &best, double decay) {
    std::vector<int> covered;
    covered.push_back(graph.size() - 2);

    double best_cost = 0;

    for (int i = 0; i < (int)best.covered.size(); ++i) {
        covered.push_back(best.covered[i]);
    }

    for (int sourceIndex = 0; sourceIndex < (int)covered.size() - 1; ++sourceIndex) {
        int source = covered[sourceIndex    ];
        int dest   = covered[sourceIndex + 1];

        best_cost += graph[source][dest].scaled_cost;
    }

    for (int sourceIndex = 0; sourceIndex < (int)covered.size() - 1; ++sourceIndex) {

        int source    = covered[sourceIndex    ];
        int dest      = covered[sourceIndex + 1];
        int destIndex = getIndexForEdge(graph, source, dest);

        double value  = ((1.0 - decay) * graph[source][destIndex].pheromone) + (decay * ( 1.0 / best_cost ));

        //printf("%4d -> %4d   %4.8f -> %4.8f\n", source, dest, value, graph[source][destIndex].pheromone);

        graph[source][destIndex].pheromone = value;
    }
}

void updateLocalPheromones ( _graph &graph, _journey candidate , double c_local_phero, double init_pheromone) {
    std::vector<int> covered;
    covered.push_back(graph.size() - 2);

    for (int i = 0; i < (int)candidate.covered.size(); ++i) {
        covered.push_back(candidate.covered[i]);
    }

    for (int sourceIndex = 0; sourceIndex < (int)covered.size() - 1; ++sourceIndex) {

        int source    = covered[sourceIndex    ];
        int dest      = covered[sourceIndex + 1];
        int destIndex = getIndexForEdge(graph, source, dest);

        double value  = ((1.0 - c_local_phero) * graph[source][destIndex].pheromone) + (c_local_phero * init_pheromone);

        //printf("%4d -> %4d   %4.8f -> %4.8f\n", source, dest, value, graph[source][destIndex].pheromone);

        graph[source][destIndex].pheromone = value;
    }
}

_journey ACOdoAntWalk ( _graph &graph, double c_heur, double c_greed ) {
    _journey candidate;
    init_journey(candidate);

    int atual = graph.size() - 2; // It starts at the first virtual node (V_0)

    //printf("Starting at %4d\n", atual);

    do {
        double prob = drand48();

        if ( prob <= c_greed ) { // Greedy choice, grabs the next task with the best (lowest) reduced cost
            //printf("Going Greedy\n");

            int bestIndex = 0;
            int dest = -1;
            double bestCost = FLT_MAX;

            for (int i = 0; i < (int)graph[atual].size(); ++i) {
                double w = graph[atual][i].cost;
                //printf("%4d %4d %4d %4.1f %4.1f\n", atual, i, graph[atual][i].dest, w, graph[atual][i].time);
                if ( w < bestCost ) {
                    bestIndex = i;
                    bestCost = w;
                    dest = graph[atual][i].dest;
                }
            }

            assert ( bestCost != FLT_MAX );

            //printf("%4d -> %4d %4.1f %4.1f\n", atual, dest, graph[atual][bestIndex].cost, graph[atual][bestIndex].time);

            candidate.cost += graph[atual][bestIndex].cost;
            candidate.time += graph[atual][bestIndex].time;
            candidate.covered.push_back(dest);

            atual = graph[atual][bestIndex].dest;
        } else { // This is the heuristic choice
            //printf("Going Heuristic\n");

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

                if ( v <= 0 ) { // This is the one
                    candidate.cost += graph[atual][step].cost;
                    candidate.time += graph[atual][step].time;
                    candidate.covered.push_back(graph[atual][step].dest);
                    atual = graph[atual][step].dest;
                }

                step ++;
                //assert(step < (int) graph[atual].size());
            } while ( v > 0 && step < (int) graph[atual].size() );
        }
        //printf(" atual = %4d\n", atual);
        validateJourney(_sp, candidate);
    } while ( atual < (int)graph.size() - 2 );

    if(debug_aco){printf(" cost = %4d time = %4d covered [", candidate.cost, candidate.time); for (int i = 0; i < (int)candidate.covered.size(); ++i) { printf("%4d, ", candidate.covered[i]); } printf("\b\b]\n");}

    return candidate;
}

_journey antColonyOptmization ( _csp *csp, _subproblem_info *sp, double *objValue ) {
    int    max_it        = 100;
    int    num_ants      =  10;
    double decay         = 0.1;
    double c_heur        = 2.5;
    double c_local_phero = 0.1;
    double c_greed       = 0.9;

    _sp = sp;

    double init_pheromone;

    _graph graph;

    _journey best;
    _journey bestUnique;
    init_journey(best);
    init_journey(bestUnique);

    graph.resize(csp->N + 2); // This has the virtual starting and ending nodes

    double minCost = FLT_MAX;

    // Copies the original graph to a graph with the info necessary to run ACO
    for (int i = 0; i < (int) csp->graph.size(); ++i) {
        for (int j = 0; j < (int) csp->graph[i].size(); ++j) {
            _path p;

            p.dest           = csp->graph[i][j].dest;
            p.cost           = sp->cost_mat[i][p.dest];
            //printf("%3d %3d %3d, %4.1f %4.1f\n", i, j, p.dest, p.cost, (float)csp->graph[i][j].cost);
            p.time           = sp->time_mat[i][p.dest];
            p.dual_cost      = sp->duals[p.dest];
            p.scaled_cost    = p.cost - sp->duals[p.dest];
            p.effective_cost = p.cost - sp->duals[p.dest];
            p.pheromone      = 0.0;

            graph[i].push_back(p);

            minCost = min(p.scaled_cost, minCost);

            //printf(" %3d -> %3d cost %5.1f time %5.1f dual %5.1f redcost %7.1f minCost %5.1f\n", i, p.dest, p.cost, p.time, p.dual_cost, p.scaled_cost, minCost);
            //printf(" %3d -> %3d cost %5.1f time %5.1f dual %5.1f redcost %7.1f minCost %5.1f\n", i, p.dest, p.cost, p.time, p.dual_cost, p.scaled_cost, minCost);
            //printf("\n");
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
        p.cost           = sp->cost_mat[csp->N][p.dest];
        p.time           = sp->time_mat[csp->N][p.dest];
        p.dual_cost      = sp->duals[p.dest];
        p.scaled_cost    = p.cost - sp->duals[p.dest];
        p.effective_cost = p.cost - sp->duals[p.dest];
        p.pheromone      = 0.0;

        graph[csp->N].push_back(p);

        minCost = min(p.scaled_cost, minCost);
    }

/*    for (int i = 0; i < (int) csp->graph.size(); ++i) {*/
        //for (int j = 0; j < (int) csp->graph[i].size(); ++j) {
            //printf(" %4d -> %4d %4.1f\n", i, csp->graph[i][j].dest, (float)csp->graph[i][j].cost);
            //printf(" %4d -> %4d %4.1f\n", i, graph[i][j].dest, graph[i][j].cost);
            //printf("\n");
        //}
/*    }*/

    if ( minCost < 0 ) { // If minCost is zero or more no useful solution can be found
        double total = 0;
        for (int i = 0; i < (int) graph.size(); ++i) {
            for (int j = 0; j < (int) graph[i].size(); ++j) {
                graph[i][j].scaled_cost += -minCost + 1; // This will make everything at least one
                total += graph[i][j].scaled_cost;
                //printf("%3d -> %3d   %7.1f\n", i, graph[i][j].dest, graph[i][j].scaled_cost);
            }
        }

        init_pheromone = 1.0 / (total * csp->N);
        init_pheromone = 1.0 / (csp->N * 327); // FIXME I just placed a random value here

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
            //printf("iter %5d ant %3d    best %4.4f  candidate %4.4f \n", iter, antIndex, getEffectiveCost(graph, best), getEffectiveCost(graph, candidate));
            //printf(" cost = %4d time = %4d covered [", candidate.cost, candidate.time); for (int i = 0; i < (int)candidate.covered.size(); ++i) { printf("%4d, ", candidate.covered[i]); } printf("\b\b]\n");

            validateJourney(sp, candidate);

            if ( getScaledCost(graph, candidate) < getScaledCost(graph, best) || ( iter + antIndex == 0 ) ) {
                //printf("New best\n");
                best = candidate;
            }

            if ( ( getScaledCost(graph, candidate) < getScaledCost(graph, bestUnique)) || ( iter + antIndex == 0 ) ) {
                //printf("New best\n");
                std::vector<int> covered;
                covered.push_back(graph.size() - 2);
                for (int i = 0; i < (int)candidate.covered.size() - 1; ++i) { covered.push_back(candidate.covered[i]); }
                if ( sp->usedJourneys[covered] == 0 ) {
                    bestUnique = candidate;
                }
            }

            //printf("%8.8f\n", init_pheromone);

            updateLocalPheromones(graph, candidate, c_local_phero, init_pheromone);
        }
        updateGlobalPheromones(graph, best, decay);
        //printf("iter %5d best %4.4f\n", iter, getEffectiveCost(graph, best));
    }

    bestUnique.covered.erase(bestUnique.covered.end() - 1);
    *objValue = getEffectiveCost (graph, bestUnique);
    if(1){printf("cost = %4d time = %4d covered [", bestUnique.cost, bestUnique.time); for (int i = 0; i < (int)bestUnique.covered.size(); ++i) { printf("%4d, ", bestUnique.covered[i]); } printf("\b\b]\n");}
    return bestUnique;

    //best.covered.erase(best.covered.end() - 1);
    //*objValue = getEffectiveCost (graph, best);

    //if(1){printf("cost = %4d time = %4d covered [", best.cost, best.time); for (int i = 0; i < (int)best.covered.size(); ++i) { printf("%4d, ", best.covered[i]); } printf("\b\b]\n");}

    //return best;
 }
