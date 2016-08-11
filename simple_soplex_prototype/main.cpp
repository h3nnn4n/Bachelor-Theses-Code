#include <iostream>
#include <vector>
#include <cstdlib>

#include <soplex.h>

#include "reader.h"
#include "types.h"
#include "printer_to_graphviz.h"
#include "backtrack.h"
#include "utils.h"

int main(int argc, char *argv[]) {
    if ( argc != 2 ) {
        printf("Missing argument\n");
        return EXIT_FAILURE;
    }

    soplex::SoPlex mysoplex;
    char input_name[256];
    char output_name[256];

    int used_journeys = 0;

    sprintf(input_name,  "%s.txt", argv[1]);
    sprintf(output_name, "%s.lp",  argv[1]);

    _csp t = file_reader(input_name);
    std::vector<_journey> journeys;

    print_to_graphviz(&t);
    return 0;

    int *vec = ( int* ) malloc ( sizeof(int) * t.N );

    for (int i = 0; i < t.N; ++i) {
        vec[i] = -1;
    }

    for (int i = 1; i <= t.N; ++i) {
        backtrack(t, i, 0, 0, vec, journeys);
    }

    print_graph(t);
    printf("\n");
    print_journeys(journeys);
    printf("\n");
    return 0;

    mysoplex.setIntParam(soplex::SoPlex::OBJSENSE, soplex::SoPlex::OBJSENSE_MINIMIZE);
    soplex::DSVector dummycol((int) journeys.size());

    for (int i = 0; i < (int) journeys.size(); ++i) {
        mysoplex.addColReal(soplex::LPCol(journeys[i].cost, dummycol, 1.0, 0.0));
    }

    std::vector<soplex::DSVector> rows;
    rows.resize( t.N + 1 );
    for (int i = 0; i < (int) journeys.size(); ++i) {
        for (int j = 0; j < (int) journeys[i].covered.size(); ++j) {
            rows[journeys[i].covered[j]].add(i, 1);
        }
    }

    soplex::DSVector row1;
    for (int i = 0; i < (int) journeys.size(); ++i) {
        row1.add(i, 1);
    }

    for (int i = 1; i <= t.N; ++i) {
        mysoplex.addRowReal(soplex::LPRow(1.0, rows[i], 1.0));
    }

    int p = 20;
    mysoplex.addRowReal(soplex::LPRow(p, row1, p));

    mysoplex.writeFileReal(output_name, NULL, NULL, NULL);

    soplex::SPxSolver::Status stat;
    soplex::DVector prim((int) journeys.size());
    soplex::DVector dual(t.N);
    stat = mysoplex.solve();

    printf("\n");
    printf("N: %4d \t maxt: %4d\n", t.N, t.time_limit);
    printf("Found %6d journeys\n", (int) journeys.size());

    if( stat == soplex::SPxSolver::OPTIMAL )
    {
        mysoplex.getPrimalReal(prim);
        mysoplex.getDualReal(dual);
        std::cout << "LP solved to optimality.\n";
        std::cout << "Objective value is " << mysoplex.objValueReal() << ".\n";
        std::cout << "Primal solution is [";
        for (int i = 0; i < (int) journeys.size() - 1; ++i) {
            std::cout << prim[i] << ", ";
        }
        std::cout << prim[(int) journeys.size() - 1] << "].\n ";

        std::cout << "Dual solution is [";
        for (int i = 0; i < t.N; ++i) {
            std::cout << dual[i] << ", ";
        }
        std::cout << "].\n";

        printf("Problem has\n%d columns\n%d rows\n", mysoplex.numColsReal(), mysoplex.numRowsReal());
        printf("Found %d possible journeys\n", (int) journeys.size());
        printf("Solution is:\n");

        for (int i = 0; i < (int) journeys.size(); ++i) {
            if ( prim[i] > 0.0 ) {
                used_journeys++;
                printf("%d %.2f : ", i, prim[i]);
                for (int j = 0; j < (int) journeys[i].covered.size(); ++j) {
                    printf("%d ", journeys[i].covered[j]);
                }
                printf("\n");
            }
        }
        printf("Using %d journeys\n", used_journeys);
    }


    return 0;
}

