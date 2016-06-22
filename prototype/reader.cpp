/* OR-Library
 * J E Beasley
 *
 * OR-Library is a collection of test data sets for a variety of OR problems.
 *
 * A full list of the test data sets available in OR-Library can be found here: http://people.brunel.ac.uk/~mastjjb/jeb/info.html
 *
 * Crew scheduling
 *
 * There are currently 10 data files.
 *
 * These data files are the ten test problems from J.E.Beasley
 * and B.Cao "A tree search algorithm for the crew scheduling
 * problem" European Journal of Operational Research 94 (1996)
 * pp517-526.
 *
 * The test problems solved in that paper are available in
 * the files csp50, csp100, csp150, csp200, csp250, csp300,
 * csp350, csp400, csp450 and csp500 (where the number for
 * each csp file is the number of tasks).
 *
 * The format of these data files is:
 * number of tasks (N), time limit
 * for each task i (i=1,...,N): start time, finish time
 * for each transition arc between two tasks (i and j):
 * i, j, cost of transition from i to j
 *
 * The value of the optimal solution for each of these data files
 * for a varying number of crews is given in the above paper.
 *
 * The largest file is csp500 of size 250Kb (approximately).
 * The entire set of files is of size 900Kb (approximately).
 *
 * Click here to access these files: http://people.brunel.ac.uk/~mastjjb/jeb/orlib/files
 *
 */
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "types.h"
#include "reader.h"

_csp file_reader(std::string name){
    std::ifstream f(name);

    _csp csp;

    f >> csp.N;
    f >> csp.time_limit;

    csp.graph.resize(csp.N + 1);

    for (int i = 0; i < csp.N; ++i) {
        int start_time, end_time;
        f >> start_time;
        f >> end_time;
        _task t;
        t.id         = i;
        t.start_time = start_time;
        t.end_time   = end_time;
        csp.task.push_back(t);
    }

    while ( f ) {
        int i;
        _edge e;
        f >> i;
        f >> e.dest;
        f >> e.cost;
        std::cout << i << '\n';
        csp.graph[i].push_back(e);
    }

    return csp;
}
