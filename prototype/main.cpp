#include <iostream>

#include "reader.h"
#include "types.h"
#include "printer_to_graphviz.h"
#include "backtrack.h"

int main(void) {
    _csp t = file_reader("csp50.txt");
    //print_to_graphviz(&t);
    int total = 0;
    //for (int i = 0; i < (int)t.start_nodes.size(); ++i)
    //std::cout << t.N << '\n';
    for (int i = 0; i < t.N; ++i) {
        total += backtrack(t, i, 0);
        //std::cout << backtrack(t, i, 0) << '\n';
    }

    std::cout << "Found " << total << " tasks\n";
    return 0;
}
