#include <cstdlib>
#include <cstdio>
#include <iostream>

#include "types.h"
#include "random.h"

// Builds a journey that covers everything
_journey all_powerful_journey(_csp *csp) {
    _journey journey;
    journey.cost = 0;

    for (int i = 0; i < (int)csp->graph.size(); ++i) {
        for (int j = 0; j < (int)csp->graph[j].size(); ++j) {
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
