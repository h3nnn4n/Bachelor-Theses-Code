#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <vector>
#include <map>

#include "simmulated_annealing.h"
#include "types.h"

_journey randomInitialSolution ( _csp *csp, _subproblem_info *sp ) {
    _journey journey;
    journey.time = 0;
    journey.cost = 0;
    journey.covered.clear();

    sp->mi = sp->mi; // Remove warning

    while ( 1 ) {
        _journey journey;
        int atual = rand() % csp->graph.size();

        while ( !csp->graph[atual].size() )
            atual = rand() % csp->graph.size();

        journey.covered.push_back(atual);
        journey.time = csp->task[atual].end_time - csp->task[atual].start_time;
        journey.cost = 0;

        //std::cout << "Starting at: " << atual << std::endl;

        while ( csp->graph[atual].size() ) {
            int x = rand() % csp->graph[atual].size();

            if ( journey.time + csp->graph[atual][x].cost > csp->time_limit )
                break;

            //std::cout << "going to " << csp->graph[atual][x].dest << " with cost " << csp->graph[atual][x].cost << std::endl;

            journey.cost += csp->graph[atual][x].cost;
            journey.time += csp->task[x].end_time - csp->task[x].start_time;
            journey.covered.push_back(csp->graph[atual][x].dest);

            atual = csp->graph[atual][x].dest;
        }

        if ( journey.covered.size() > 1 ) {
            //std::cout << "size " << journey.covered.size() << std::endl;
            printf("Using initial solution cost = %4d time = %4d\n", journey.cost, journey.time);
            printf("Covered [");

            // Walks throught the new journey and builds the coeffs for the column
            for (int i = 0; i < (int)journey.covered.size(); ++i) {
                printf("%4d, ", journey.covered[i]);
            }

            printf("\b\b]\n");

            //exit(0);
            return journey;
        }
    }

    return journey;
}

double objectiveFuntion ( _csp *csp, _subproblem_info *sp, _journey &journey ) {
    double value = 0;

    value -= sp->mi;

    for (int i = 0; i < (int) journey.covered.size(); ++i) {
        value -= sp->duals[journey.covered[i]];
    }

    for (int i = 1; i < (int) journey.covered.size(); ++i) {
        value += sp->cost_mat[journey.covered[i-1]][journey.covered[i]];
    }

    // ^ This is the pure reduced costs, but we also have constrains;
    // Each violated constraint should have a weight big enough to be worth
    // considering but not so big that is overwhelms the reduced_cost

    if ( journey.time > csp->time_limit ) {
        double x = 1.0 - ((csp->time_limit - (journey.time - csp->time_limit)) / (double)csp->time_limit);
        double penalty = x*x;
        double penaltyWeigth = csp->time_limit;

        printf("Energy is %2.4f with a penalty of %2.8f %2.8f\n", value, penalty, x);

        value -= penalty * penaltyWeigth;

        printf("Final energy is %2.4f\n", value);
    }

    return value;
}

_journey simmulatedAnnealing ( _csp *csp, _subproblem_info *sp, double *objValue ) {
    double t_0   = 50.0;
    double t_n   = 0.0;
    double dt    = 0;
    double temp;
    double energy;
    int iter     = 0;
    int max_iter = 1000;

    *objValue = 0;
    dt = (t_n - t_0 ) / max_iter;
    temp = t_0;

    _journey solution = randomInitialSolution(csp, sp);

    for (iter = 0; iter < max_iter; ++iter) {
        energy = objectiveFuntion(csp, sp, solution);
        printf(" temp = %4.4f  covered [", temp); for (int i = 0; i < (int)solution.covered.size(); ++i) { printf("%4d, ", solution.covered[i]); } printf("\b\b]\n");

        temp += dt;
    }

    exit(0);

    return solution;
}
