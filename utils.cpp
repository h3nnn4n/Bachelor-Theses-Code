#include <vector>
#include <cstdio>
#include <assert.h>

#include "types.h"
#include "utils.h"

void print_journeys(std::vector<_journey> &journeys) {
    for (int i = 0; i < (int) journeys.size(); ++i) {
        printf("%4d \t%6d %6d: ", i, journeys[i].cost, journeys[i].time);
        for (int j = 0; j < (int) journeys[i].covered.size(); ++j) {
            printf("%d ", journeys[i].covered[j]);
        }
        printf("\n");
    }

    return;
}

void print_graph(_csp csp) {
    for (int i = 0; i < (int)csp.graph.size(); ++i) {
        for (int j = 0; j < (int)csp.graph[i].size(); ++j) {
            printf("%2d -> %2d\n", i, csp.graph[i][j].dest);
        }
    }
}

void init_journey( _journey &journey) {
    journey.cost = 0;
    journey.time = 0;
    journey.covered.clear();
}

void update_used_journeys_with_vector(_subproblem_info &subproblemInfo, std::vector<_journey> &journeys) {
    for (auto journey : journeys) {
        update_used_journeys(subproblemInfo, journey);
    }
}

bool update_used_journeys(_subproblem_info &subproblemInfo, _journey journey) {
    if ( subproblemInfo.usedJourneys.count(journey.covered) == 0 ) {
        subproblemInfo.journeys.push_back(journey);
        subproblemInfo.usedJourneys[journey.covered] = true;
        return true;
    } else {
        return false;
    }
}

#if 0
static int getIndexForEdge( _csp &csp, int a, int b) {
    for (int i = 0; i < (int) csp.graph[a].size(); ++i) {
        if ( csp.graph[a][i].dest == b ) {
            return i;
        }
    }
    return -1;
}
#endif

void validateJourney ( _subproblem_info *sp, _journey &journey) {
    if ( journey.covered.size() == 0 ) {
        assert( journey.cost == 0 && "Journey cost should be zero");
        assert( journey.time == 0 && "Journey time should be zero");
        return;
    }

    double cost = 0;
    double time = 0;

    cost += sp->cost_mat[sp->N][journey.covered[0]];
    time += sp->time_mat[sp->N][journey.covered[0]];

    for (int i = 0; i < (int)journey.covered.size(); ++i) {
        if ( i < (int)journey.covered.size() - 1) {
            int source = journey.covered[i    ];
            int dest   = journey.covered[i + 1];

            cost += sp->cost_mat[source][dest];
            time += sp->time_mat[source][dest];
        }
    }

    if ( cost != journey.cost || time != journey.time ) {
        for (int i = 0; i < (int)journey.covered.size(); ++i) {
            if ( i < (int)journey.covered.size() - 1) {
                int source = journey.covered[i    ];
                int dest   = journey.covered[i + 1];
                printf("%4d -> %4d cost %7.2f time %7.2f\n", source, dest, sp->cost_mat[source][dest], sp->time_mat[source][dest]);
            }
        }

        puts("Found a journey with invalid values");
        printf("%4.1f %4.1f %4.1f %4.1f\n", (double)cost, (double)journey.cost, (double)time, (double)journey.time);
        exit(0);
    }
}

float calculateJourneyReducedCost ( _subproblem_info *sp, _journey &journey) {
    double reducedCost = 0;

    reducedCost += sp->cost_mat[sp->N][journey.covered[0]];
    reducedCost -= sp->duals[journey.covered[0]];
    reducedCost -= sp->mi;

    for (int i = 0; i < (int)journey.covered.size(); ++i) {
        if ( i < (int)journey.covered.size() - 1) {
            int source = journey.covered[i    ];
            int dest   = journey.covered[i + 1];

            reducedCost += sp->cost_mat[source][dest];
            reducedCost -= sp->duals[dest];
        }
    }

    return reducedCost;
}
