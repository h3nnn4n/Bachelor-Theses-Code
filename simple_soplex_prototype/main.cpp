#include <iostream>
#include <vector>
#include <cstdlib>

#include <soplex.h>

#include "reader.h"
#include "types.h"
#include "printer_to_graphviz.h"
#include "backtrack.h"
#include "utils.h"

int main(void) {
    soplex::SoPlex mysoplex;

    //bool max_journey = false;
    //int total_journeys = 0;

    std::vector<int>    ia;
    std::vector<int>    ja;
    std::vector<double> ar;

    ia.push_back(0);
    ja.push_back(0);
    ar.push_back(0);

    _csp t = file_reader("csp5.txt");
    std::vector<_journey> journeys;

    //print_to_graphviz(&t);
    //return 0;

    int *vec = ( int* ) malloc ( sizeof(int) * t.N );
    //for (int i = 0; i < (int)t.start_nodes.size(); ++i)
    //std::cout << t.N << '\n';

    for (int i = 0; i < t.N; ++i) {
        vec[i] = -1;
    }

    for (int i = 1; i <= t.N; ++i) {
        backtrack(t, i, 0, 0, vec, journeys);
    }

    //print_graph(t);
    //printf("\n");
    //print_journeys(journeys);
    //printf("\n");
    //return 0;

    mysoplex.setIntParam(soplex::SoPlex::OBJSENSE, soplex::SoPlex::OBJSENSE_MINIMIZE);
    soplex::DSVector dummycol((int) journeys.size());

    for (int i = 0; i < (int) journeys.size(); ++i) {
        mysoplex.addColReal(soplex::LPCol(journeys[i].cost, dummycol, 1.0, 0.0));
    }

    std::vector<soplex::DSVector> rows;
    rows.resize((int) journeys.size()+1);
    for (int i = 0; i < (int) journeys.size(); ++i) {
        for (int j = 0; j < (int) journeys[i].covered.size(); ++j) {
            rows[journeys[i].covered[j]].add(i, 1);
        }
    }

    for (int i = 1; i <= (int) journeys.size(); ++i) {
        mysoplex.addRowReal(soplex::LPRow(1.0, rows[i], 1.0));
    }

    mysoplex.writeFileReal("csp5.lp", NULL, NULL, NULL);

    soplex::SPxSolver::Status stat;
    soplex::DVector prim((int) journeys.size());
    soplex::DVector dual(t.N);
    stat = mysoplex.solve();

    if( stat == soplex::SPxSolver::OPTIMAL )
    {
        mysoplex.getPrimalReal(prim);
        mysoplex.getDualReal(dual);
        std::cout << "LP solved to optimality.\n";
        std::cout << "Objective value is " << mysoplex.objValueReal() << ".\n";
        std::cout << "Primal solution is [";
        for (int i = 0; i < (int) journeys.size(); ++i) {
            std::cout << prim[i] << ", ";
        }
        std::cout << "].\n";

        std::cout << "Dual solution is [";
        for (int i = 0; i < t.N; ++i) {
            std::cout << dual[i] << ", ";
        }
        std::cout << "].\n";
    }

    //printf("Problem has\n%d columns\n%d rows\n", (int)journeys.size(), t.N);
    //printf("Found %d possible journeys\n", (int) journeys.size());
    //printf("Optimal solution is: %.3f with %d journeys\n", glp_get_obj_val(lp), total_journeys);

    //std::cout << "Found " << glp_get_obj_val(lp) << " tasks\n";
    return 0;
}
