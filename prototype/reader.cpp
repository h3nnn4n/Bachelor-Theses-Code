#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "types.h"
#include "reader.h"

_csp file_reader(std::string name){
    std::ifstream f(name);

    _csp csp50;

    f >> csp50.N;
    f >> csp50.time_limit;

    csp50.graph.resize(csp50.N + 1);

    for (int i = 0; i < csp50.N; ++i) {
        int start_time, end_time;
        f >> start_time;
        f >> end_time;
        _task t;
        t.id         = i;
        t.start_time = start_time;
        t.end_time   = end_time;
        csp50.task.push_back(t);
    }

    while ( f ) {
        int i;
        _edge e;
        f >> i;
        f >> e.dest;
        f >> e.cost;
        std::cout << i << '\n';
        csp50.graph[i].push_back(e);
    }

    return csp50;
}
