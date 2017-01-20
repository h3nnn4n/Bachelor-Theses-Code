#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>

#include "types.h"
#include "random.h"

// Builds a full feasible solution to the master problem using a simple random/greddy heuristic
void build_heur_sol ( _csp *csp, std::vector<_journey> &journeys ) {
    // Inits the journey vector
    for (int i = 0; i < csp->n_journeys; ++i) {
        _journey j;
        j.cost = 0;
        j.time = 0;
        journeys.push_back(j);
    }

    int abort = 1000; // Number of tries before aborting

    // Runs until a solution is found;
    bool solution_found;
    do {
        //printf("\n NEW STEP\n");
        // Resets the journey vector
        for (int i = 0; i < csp->n_journeys; ++i) {
            journeys[i].cost = 0;
            journeys[i].time = 0;
            journeys[i].covered.clear();
        }

        std::vector<bool> used; // Creates a vector to map what is already covered during the first phase
        std::vector<bool> covered; // Creates a vector to map what is already covered and what is not
        used.resize(csp->N);
        covered.resize(csp->N);
        for (int i = 0; i < (int)used.size(); ++i) {
            used[i] = false;
            covered[i] = false;
        }

        // Sets one task as starting point for each journey
        for (int i = 0; i < csp->n_journeys ; ++i) {
            bool try_again = true;
            do { // Loops until an empty espace is found
                int x = rand() % csp->N;
                if ( !used[x] ) {
                    try_again = false;
                    used[x] = true;
                    covered[x] = true;
                    //printf("%2d = ", x);
                }
            } while ( try_again );
            //printf("%2d\n", i);
        }

        // Makes the journeys with the starting points found above
        int pivot = 0;
        for (int i = 0; i < csp->n_journeys ; ++i) {
            for ( ; !used[pivot]; pivot++ );
            journeys[i].covered.push_back(pivot);
            pivot++;
        }

        bool found_something = true;

        // loops while it is possible to add something to the current solution
        do {
            found_something = false;

            for (int i = 0; i < (int)journeys.size(); ++i) {
                // Picks the last covered task on the current journey
                int atual = journeys[i].covered[(int)journeys[i].covered.size() - 1];

                //printf("i = %2d  atual = %2d\n", i, atual);

                if ( csp->graph[atual].size() == 0 ) { // It is an end point
                    //printf("endpoint reset\n");
                    continue;                          // Cant start from here
                }

                int x;
                int dest;
                int max_tries = 50;
                do {
                    x    = rand() % csp->graph[atual].size();
                    dest = csp->graph[atual][x].dest;

                    if ( --max_tries == 0 )
                        break;

                    if ( covered[dest] ) {
                        ////printf(" %2d -> %2d is already covered\n", atual, dest);
                    }

                } while ( journeys[i].time + csp->graph[atual][dest].cost > csp->time_limit || covered[dest] );

                if ( max_tries == 0 ) {
                    //printf("max_tries reset\n");
                    continue;
                }

                //std::cout << "going to " << csp->graph[atual][x].dest << " with cost " << csp->graph[atual][x].cost << std::endl;

                journeys[i].cost += csp->graph[atual][dest].cost;
                journeys[i].time += csp->task[dest].end_time - csp->task[dest].start_time;
                journeys[i].covered.push_back(dest);

                covered[dest]  = true;
                found_something = true;
                //printf("Added dest = %3d\n", dest);
            }
        } while ( found_something );

        // Checks if everything was covered
        solution_found = true;
        for (int i = 0; i < (int)covered.size(); ++i) {
            if ( !covered[i] ) {
                solution_found = false;
                //printf("%2d is not covered\n", i);
                break;
            }
        }

        if ( --abort == 0 ) {
            fprintf(stderr, "Max tried exceed. Aborting\n");
            break;
        }

    } while ( !solution_found );

    fprintf(stderr, "Found a feasible solution\n");
}

// Builds a journey that covers everything
_journey all_powerful_journey(_csp *csp) {
    _journey journey;
    journey.cost = 0;

    for (int i = 0; i < (int)csp->graph.size(); ++i) {
        for (int j = 0; j < (int)csp->graph[i].size(); ++j) {
            journey.cost += csp->graph[i][j].cost;
        }
    }

    for (int i = 0; i < csp->N; ++i) {
        journey.covered.push_back(i);
    }

    journey.cost *= 2;

    return journey;
}

_journey random_journey(_csp *csp) {
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
            //std::cout << journey.covered.size() << std::endl;
            return journey;
        } else {

        }
    }
}

bool test_random_journey_from(_csp *csp, int start_from) {
    if ( csp->graph[start_from].size() == 0 )
        return false;
    return true;
}

_journey random_journey_from(_csp *csp, int start_from) {
    int tries = 0;
    while ( tries++ <= 50 ) {
        _journey journey;
        int atual = start_from;

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
            //std::cout << journey.covered.size() << std::endl;
            return journey;
        } else {

        }
    }

    std::cout << "Failed to find a solution. Aborting." << std::endl;
    abort();
}

_journey random_journey_to(_csp *csp, int start_from) {
    _journey p;

    p.cost = 0;
    p.time = csp->task[start_from].end_time - csp->task[start_from].start_time;

    p.covered.push_back(start_from);

    for (int i = 0; i < (int) csp->graph.size(); ++i) {
        for (int j = 0; j < (int) csp->graph[i].size(); ++j) {
            if ( csp->graph[i][j].dest == start_from ) {
                if ( csp->task[i].end_time - csp->task[i].start_time + p.time <= csp->time_limit ) {
                    p.cost += csp->graph[i][j].cost;
                    p.time += csp->task[i].end_time - csp->task[i].start_time;
                    p.covered.push_back(i);
                    return p;
                }
            }
        }
    }

    std::cout << "I should not be here" << std::endl;
    abort();
    return p;
}
