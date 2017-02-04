#include <assert.h>

#include <iostream>

#include "types.h"

bool debug_sa = false;

_journey randomInitialSolution ( _csp *csp, _subproblem_info *sp ) {
    _journey journey;
    journey.time = 0;
    journey.cost = 0;
    journey.covered.clear();

    sp->mi = sp->mi; // Remove warning

    while ( 1 ) {
        _journey journey;
        int atual;

        do {
            atual = rand() % csp->graph.size();
        } while ( !csp->graph[atual].size() );

        journey.covered.push_back(atual);
        //journey.time = csp->task[atual].end_time - csp->task[atual].start_time;
        journey.time = sp->time_mat[csp->N][atual];
        journey.cost = 0; 
        std::cout << "Starting at: " << atual << " with time " << journey.time << std::endl;

        while ( csp->graph[atual].size() ) {
            int x = rand() % csp->graph[atual].size();
            int dest = csp->graph[atual][x].dest;

            if ( journey.time + sp->time_mat[atual][dest] > csp->time_limit )
                break;

            std::cout << "going to " << csp->graph[atual][x].dest << " with cost " << csp->graph[atual][x].cost << " and time " << sp->time_mat[atual][dest] << std::endl;

            if ( sp->time_mat[atual][dest] < 0 ) {
                fprintf(stderr, "%3d -> %3d = %3.3f\n", atual, dest, sp->time_mat[atual][dest]);
            }
            assert ( sp->time_mat[atual][dest] >= 0 && "Invalid time");

            journey.cost += csp->graph[atual][x].cost;
            //journey.cost += sp->cost_mat[atual][x];//csp->graph[atual][x].cost;
            journey.time += sp->time_mat[atual][dest];//csp->task[x].end_time - csp->task[x].start_time;
            journey.covered.push_back(csp->graph[atual][x].dest);

            atual = csp->graph[atual][x].dest;
        }

        if ( journey.covered.size() > 1 ) {
            //std::cout << "size " << journey.covered.size() << std::endl;
            if ( debug_sa ) printf("Using initial solution cost = %4d time = %4d\n", journey.cost, journey.time);
            if ( debug_sa ) printf("Covered [");

            // Walks throught the new journey and builds the coeffs for the column
            for (int i = 0; i < (int)journey.covered.size(); ++i) {
                if ( debug_sa ) printf("%4d, ", journey.covered[i]);
            }

            if ( debug_sa ) printf("\b\b]\n");

            //exit(0);
            return journey;
        }
    }

    return journey;
}

double objectiveFuntion ( _csp *csp, _subproblem_info *sp, _journey &journey ) {
    double value = 0;

    value -= sp->mi;

    //printf("%4.2f\n", value);

    for (int i = 0; i < (int) journey.covered.size(); ++i) {
        value -= sp->duals[journey.covered[i]];
        //printf("%4.2f\n", value);
    }

    for (int i = 1; i < (int) journey.covered.size(); ++i) {
        value += sp->cost_mat[journey.covered[i-1]][journey.covered[i]];
        //printf("%4.2f\n", value);
    }

    // ^ This is the pure reduced costs, but we also have constrains;
    // Each violated constraint should have a weight big enough to be worth
    // considering but not so big that is overwhelms the reduced_cost

    if ( journey.time > csp->time_limit ) {
        double x = (journey.time - csp->time_limit);
        double penalty = x;
        double penaltyWeigth = 2.0;

        //double x = 1.0 - ((csp->time_limit - (journey.time - csp->time_limit)) / (double)csp->time_limit);
        //double penalty = x*x;
        //double penaltyWeigth = csp->time_limit;

        if ( debug_sa ) printf("Energy is %2.4f with a penalty of %2.8f time = %4d\n", value, penalty, journey.time);

        value += penalty * penaltyWeigth;

        if ( debug_sa ) printf("Final energy is %2.4f\n", value);
    }

    return value;
}

void appendTaskToJourneyBeginning ( _csp *csp, _subproblem_info *sp, _journey &journey ) {
    int atual = journey.covered[0];

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

    if ( debug_sa ) printf("Inserting at the begining %3d -> %3d cost = %3.1f time = %3.1f\n", atual, dest, sp->cost_mat[atual][dest], sp->time_mat[atual][dest]);

    journey.covered.insert(journey.covered.begin(), atual);
    journey.cost += sp->cost_mat[atual][dest];

    journey.time += sp->time_mat[atual][dest];   // Adds the time for the atual task and the time between dest and atual
    journey.time -= sp->time_mat[csp->N][dest];  // Remoevs the time for the dest task
    journey.time += sp->time_mat[csp->N][atual]; // Adds the time for the atual task
}

void appendTaskToJourneyEnd ( _csp *csp, _subproblem_info *sp, _journey &journey ) {
    int atual = journey.covered[journey.covered.size() - 1];

    if ( csp->graph[atual].size() == 0 )
        return;

    int next = rand() % csp->graph[atual].size();
    int dest = csp->graph[atual][next].dest;

    if ( debug_sa ) printf("Inserting at the end %3d -> %3d cost = %3.1f time = %3.1f\n", atual, dest, sp->cost_mat[atual][dest], sp->time_mat[atual][dest]);

    journey.covered.push_back(dest);
    journey.cost += sp->cost_mat[atual][dest];
    journey.time += sp->time_mat[atual][dest];
}

void removeFirstTaskFromJourney ( _csp *csp, _subproblem_info *sp, _journey &journey){
    if ( journey.covered.size() <= 1 ) return;

    int toDelete = journey.covered[0];
    int next     = journey.covered[1];

    int deltaTime = 0;

    deltaTime -= sp->time_mat[csp->N][toDelete];
    deltaTime -= sp->time_mat[toDelete][next];
    deltaTime += sp->time_mat[csp->N][next];

    journey.time += deltaTime;

    if ( debug_sa ) printf("Removing First Task %3d -> %3d cost = %4.1f time = %4d\n", toDelete, next, sp->cost_mat[toDelete][next], deltaTime);

    journey.cost -= sp->cost_mat[toDelete][next];
    journey.covered.erase(journey.covered.begin());
}

void removeLastTaskFromJourney ( _csp *csp, _subproblem_info *sp, _journey &journey){
    if ( journey.covered.size() <= 1 ) return;

    csp->N = csp->N; // Remove boring warning

    if ( debug_sa ) printf("Removing Last Task\n");

    int toDelete = journey.covered[journey.covered.size() - 1];
    int before   = journey.covered[journey.covered.size() - 2];

    journey.time -= sp->time_mat[before][toDelete];
    journey.cost -= sp->cost_mat[before][toDelete];
    journey.covered.erase(journey.covered.end() - 1);
}
