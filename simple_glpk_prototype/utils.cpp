#include <vector>
#include <cstdio>
#include "types.h"
#include "utils.h"

void print_journeys(std::vector<_journey> &journeys) {
    for (int i = 0; i < (int) journeys.size(); ++i) {
        printf("%4d \t%6d: ", i, journeys[i].cost);
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
