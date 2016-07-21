#include <iostream>
#include <vector>
#include <cstdlib>

#include "reader.h"
#include "types.h"
#include "printer_to_graphviz.h"
#include "backtrack.h"

int main(void) {
    _csp t = file_reader("csp50.txt");
    std::vector<_journey> journeys;
    //print_to_graphviz(&t);
    int total = 0;
    int *vec = ( int* ) malloc ( sizeof(int) * t.N );
    //for (int i = 0; i < (int)t.start_nodes.size(); ++i)
    //std::cout << t.N << '\n';
    for (int i = 0; i < t.N; ++i) {
        total += backtrack(t, i, 0, 0, vec, journeys);
        //std::cout << backtrack(t, i, 0) << '\n';
    }

    for (int i = 0; i < (int) journeys.size(); ++i) {
        printf("%d: ", journeys[i].cost);
        for (int j = 0; j < (int) journeys[i].covered.size(); ++j) {
            printf("%d ", journeys[i].covered[j]);
        }
        printf("\n");
    }

    std::cout << "Found " << total << " tasks\n";
    return 0;
}

