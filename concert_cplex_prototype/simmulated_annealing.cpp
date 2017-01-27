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
        double x = (journey.time - csp->time_limit);
        double penalty = x;
        double penaltyWeigth = 1.0;

        //double x = 1.0 - ((csp->time_limit - (journey.time - csp->time_limit)) / (double)csp->time_limit);
        //double penalty = x*x;
        //double penaltyWeigth = csp->time_limit;

        printf("Energy is %2.4f with a penalty of %2.8f %2.8f\n", value, penalty, x);

        value += penalty * penaltyWeigth;

        printf("Final energy is %2.4f\n", value);
    }

    return value;
}

void appendTaskToJourneyBeginning ( _csp *csp, _subproblem_info *sp, _journey &journey ) {
    int atual = journey.covered[journey.covered.size() - 1];

    std::vector<int> leadingTo;

    for (int i = 0; i < (int)csp->graph.size(); ++i) {
        for (int j = 0; j < (int)csp->graph[i].size(); ++j) {
            if ( csp->graph[i][j].dest == atual ) {
                leadingTo.push_back( i );
            }
        }
    }

    if ( leadingTo.size() == 0 )
        return;

    int index = rand() % leadingTo.size();
    int dest = atual;
    atual = leadingTo[index];

    printf("Inserting at the begining %3d -> %3d cost = %3.1f time = %3.1f\n", atual, dest, sp->cost_mat[atual][dest], sp->time_mat[atual][dest]);

    journey.covered.insert(journey.covered.begin(), atual);
    journey.cost += sp->cost_mat[atual][dest];
    journey.time += sp->time_mat[atual][dest];
}

void appendTaskToJourneyEnd ( _csp *csp, _subproblem_info *sp, _journey &journey ) {
    int atual = journey.covered[journey.covered.size() - 1];

    if ( csp->graph[atual].size() == 0 )
        return;

    int next = rand() % csp->graph[atual].size();
    int dest = csp->graph[atual][next].dest;

    printf("Inserting at the end %3d -> %3d cost = %3.1f time = %3.1f\n", atual, dest, sp->cost_mat[atual][dest], sp->time_mat[atual][dest]);

    journey.covered.push_back(dest);
    journey.cost += sp->cost_mat[atual][dest];
    journey.time += sp->time_mat[atual][dest];
}

void removeFirstTaskFromJourney ( _csp *csp, _subproblem_info *sp, _journey &journey){
    if ( journey.covered.size() <= 1 ) return;

    int toDelete = journey.covered[0];
    int next     = journey.covered[1];

    journey.time -= (csp->task[toDelete].end_time - csp->task[toDelete].start_time);
    journey.time -= sp->time_mat[toDelete][next];
    journey.time += (csp->task[next].end_time - csp->task[next].start_time);

    journey.cost -= sp->cost_mat[toDelete][next];

    journey.covered.erase(journey.covered.begin());
}

void removeLastTaskFromJourney ( _csp *csp, _subproblem_info *sp, _journey &journey){
    if ( journey.covered.size() <= 1 ) return;

    csp->N = csp->N; // Remove boring warning

    int toDelete = journey.covered[journey.covered.size() - 1];
    int before   = journey.covered[journey.covered.size() - 2];

    journey.time -= sp->time_mat[before][toDelete];

    journey.cost -= sp->cost_mat[before][toDelete];

    journey.covered.erase(journey.covered.end());
}

_journey simmulatedAnnealing ( _csp *csp, _subproblem_info *sp, double *objValue ) {
    double t_0   = 50.0;
    double t_n   = 0.0;
    double dt    = 0;
    double temp;
    double energy;
    int iter     = 0;
    int max_iter = 10;

    //double chanceToRemove = 0.1;
    //double chanceToAppend = 0.1;

    *objValue = 0;
    dt = (t_n - t_0 ) / max_iter;
    temp = t_0;

    _journey solution = randomInitialSolution(csp, sp);

    for (iter = 0; iter < max_iter; ++iter) {
        printf("\n");
        _journey candidate = solution;
        //double p = drand48();
        //if ( p < chanceToRemove ) {
            //int toDelete = 0;
            //if ( rand() % 2 == 0 ) {
                ////toDelete = candidate.
            //}
        //} else if ( p < chanceToAppend + chanceToRemove ) {

        //}
        if ( rand() % 2 == 0 ) {
            appendTaskToJourneyEnd(csp, sp, candidate);
        } else {
            appendTaskToJourneyBeginning(csp, sp, candidate);
        }

        energy = objectiveFuntion(csp, sp, solution);
        double candidateEnergy = objectiveFuntion(csp, sp, candidate);

        if ( candidateEnergy < energy ) {
            solution = candidate;
        }

        printf(" temp = %4.4f  covered [", temp); for (int i = 0; i < (int)solution.covered.size(); ++i) { printf("%4d, ", solution.covered[i]); } printf("\b\b]\n");

        temp += dt;
    }

    exit(0);

    return solution;
}
