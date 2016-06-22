#include <iostream>

#include "printer_to_graphviz.h"
#include "types.h"

void print_to_graphviz(_csp *csp){
    std::cout << "digraph world {"
                 "size=\"7,7\";";

    for (int i = 0; i < (int)csp->graph.size(); ++i) {
        for (int j = 0; j < (int)csp->graph[i].size(); ++j) {
            std::cout << i << " -> " << csp->graph[i][j].dest << ";\n";
        }
    }

    std::cout << "}\n";

    return;
}
