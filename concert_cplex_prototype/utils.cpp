#include <vector>
#include <cstdio>

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

static int getIndexForEdge( _csp &csp, int a, int b) {
    for (int i = 0; i < (int) csp.graph[a].size(); ++i) {
        if ( csp.graph[a][i].dest == b ) {
            return i;
        }
    }
    return -1;
}

void validateJourney ( _subproblem_info *sp, _journey &journey) {
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
                printf("%4d -> %4d\n", source, dest);
            }
        }

        puts("Found a journey with invalid values");
        printf("%4.1f %4.1f %4.1f %4.1f\n", (double)cost, (double)journey.cost, (double)time, (double)journey.time);
        exit(0);
    }
}
