#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <cassert>

#include "types.h"
#include "random.h"

//#include "types.h"
//#include "utils.h"

//_journey journey;
//init_journey(journey);

//journey.cost += sp->cost_mat[csp->N][0];
//journey.time += sp->time_mat[csp->N][0];
//journey.covered.push_back(0);

//journey.cost += sp->cost_mat[0][10];
//journey.time += sp->time_mat[0][10];
//journey.covered.push_back(10);

// Builds a full feasible solution to the master problem using a simple random/greddy heuristic
void build_heur_sol ( _csp *csp, std::vector<_journey> &journeys ) {
    //std::vector<_journey> journeys;

    //journeys.resize(csp->n_journeys);

    // Inits the journey vector
    for (int i = 0; i < csp->n_journeys; ++i) {
        _journey j;
        j.cost = 0;
        j.time = 0;
        journeys.push_back(j);
    }

    int tries = 100000;
    int abort = tries; // Number of tries before aborting

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
        // This parts selects all tasks that have a incidence degree of zero
        for (int i = 0; i < (int)csp->start_nodes.size(); ++i) {
            int x = csp->start_nodes[i];
            used[x] = true;
            covered[x] = true;
            //printf("%2d = %2d\n", x, i);
        }

        //printf("--\n");

        // This part selects the rest randomly
        for (int i = (int)csp->start_nodes.size(); i < csp->n_journeys ; ++i) {
            bool try_again = true;
            do { // Loops until an empty espace is found
                int x = ( (int)csp->start_nodes.size() + rand() ) % csp->N;
                if ( !used[x] ) {
                    try_again = false;
                    used[x] = true;
                    covered[x] = true;
                    //printf("%2d = ", x);
                }
            } while ( try_again );
            //printf("%2d\n", i);
        }

        // This Piece of code makes sure that every thing is setup correctly
        int w = 0;
        for (int i = 0; i < csp->N; ++i) { if ( covered[i] ) w += 1; }
        if ( w != csp->n_journeys ) { printf("%3d %3d\n", w, csp->n_journeys); }
        assert ( w == csp->n_journeys );

        // Makes the journeys with the starting points found above
        int pivot = 0;
        for (int i = 0; i < csp->n_journeys ; ++i) {
            for ( ; !used[pivot]; pivot++ );
            journeys[i].covered.push_back(pivot);
            journeys[i].time += csp->task[pivot].end_time - csp->task[pivot].start_time;
            pivot++;
        }

        bool found_something = true;

        //puts("");
        // loops while it is possible to add something to the current solution
        do {
            found_something = false;

            // This looks for a journey using the alread found tasks as the starting point (it goes foward)
            for (int i = 0; i < (int)journeys.size(); ++i) {
                // Picks the last covered task on the current journey
                int atual = journeys[i].covered[(int)journeys[i].covered.size() - 1];

                //printf("i = %2d  atual = %2d\n", i, atual);

                if ( csp->graph[atual].size() == 0 ) { // It is an end point
                    //printf("endpoint reset\n");
                    continue;                          // Cant start from here
                }

                int x = -1;
                int dest;
                int max_tries = 50;
                do {
                    x += 1;
                    //x    = rand() % csp->graph[atual].size();
                    dest = csp->graph[atual][x].dest;

                    if ( x == (int) csp->graph[atual].size() ) {
                        max_tries = 1;
                    }

                    if ( --max_tries == 0 )
                        break;

                    if ( covered[dest] ) {
                        ////printf(" %2d -> %2d is already covered\n", atual, dest);
                    }

                    if ( journeys[i].time + (csp->task[dest].end_time - csp->task[atual].end_time) > csp->time_limit ) {
                        //printf("TIEM LIMIT: %4d\n", journeys[i].time + (csp->task[dest].end_time - csp->task[atual].end_time));
                    }

                } while ( journeys[i].time + (csp->task[dest].end_time - csp->task[atual].end_time) > csp->time_limit || covered[dest] );

                if ( max_tries == 0 ) {
                    //printf("max_tries reset\n");
                    continue;
                }

                //std::cout << atual << " is going to " << csp->graph[atual][x].dest << " with cost " << csp->graph[atual][x].cost << std::endl;

                if ( csp->graph[atual][x].cost == 0 ) {
                    printf(" # %3d -> %3d has cost = %3d\n", atual, dest, csp->graph[atual][x].cost);
                    //printf(" @ %3d -> %3d has cost = %3d\n", dest, atual, csp->graph[dest][atual].cost);
                }

                journeys[i].cost += csp->graph[atual][x].cost;
                journeys[i].time += csp->task[dest].end_time - csp->task[atual].end_time;
                journeys[i].covered.push_back(dest);

                covered[dest]   = true;
                found_something = true;
                //printf("Added dest = %3d\n", dest);
            }

            // This looks for an edge to cover something leading to the journey
            // Looks for an uncovered task
            int uncovered = -1;
            for (int i = 0; i < (int)covered.size(); ++i) {
                if ( !covered[i] ) {
                    uncovered = i;
                    break;
                }
            }

            if ( uncovered != -1 ) {
                //printf("%2d is uncovered\n", uncovered);
                for (int i = 0; i < (int)csp->graph[uncovered].size(); ++i) {
                    int dest = csp->graph[uncovered][i].dest;
                    //printf("%2d goes to %2d\n", uncovered, dest);

                    // Needs to check that it will go to a task in the beggining of a journey
                    bool goingToFirstTask = false;
                    for (int j = 0; j < (int)journeys.size(); ++j) {
                        if ( journeys[j].covered[0] == dest ) {
                            goingToFirstTask = true;
                            break;
                        }
                    }

                    if ( !goingToFirstTask ) {
                        continue;
                    }

                    //int j_index = -1;
                    for (int k = 0; k < (int)journeys.size(); ++k) {
                        for (int j = 0; j < (int)journeys[k].covered.size(); ++j) {
                            if ( journeys[k].covered[j] == dest ) {
                                //j_index = j;
                                if ( journeys[k].time + (csp->task[dest].start_time - csp->task[uncovered].start_time) <= csp->time_limit) {

                                    journeys[k].time += (csp->task[dest].start_time - csp->task[uncovered].start_time);

                                    journeys[k].cost += csp->graph[uncovered][i].cost;

                                    //journeys[k].covered.push_back(uncovered);
                                    journeys[k].covered.insert(journeys[k].covered.begin(), uncovered);

                                    if ( csp->graph[uncovered][i].cost == 0 ) {
                                        printf(" + %3d -> %3d has cost = zero\n", uncovered, dest);
                                        exit(0);
                                    }

                                    covered[uncovered] = true;
                                    found_something = true;
                                    goto outtahere;
                                }
                            }
                        }
                    }
                }
outtahere:;
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
            fprintf(stderr, "Max tries exceed. Aborting\n");
            exit(-1);
            //break;
        }

    } while ( !solution_found );

    fprintf(stderr, "Found a feasible solution after %d tries \n", tries - abort);
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
