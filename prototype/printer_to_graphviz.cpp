#include <iostream>

#include "printer_to_graphviz.h"
#include "types.h"

void print_to_graphviz(_csp *csp){
    std::cout << "digraph world {\n"
                 //"edge[weight=0.1];"
                 "nodesep=0.1;"
                 "splines=compound;"
                 "concentrate=true;"
                 //"overlap_shrink=true;"
                 //"sep=\"+25,25\";"
                 "overlap=ortho;";

    for (int i = 0; i < (int)csp->graph.size(); ++i) {
        for (int j = 0; j < (int)csp->graph[i].size(); ++j) {
            std::cout << i << " -> " << csp->graph[i][j].dest << ";\n";
        }
    }

    std::cout << "}\n";

    return;
}
