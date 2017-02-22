#include <iostream>

#include "random.h"
#include "reader.h"
#include "types.h"
#include "utils.h"


int main(int argc, char *argv[]) {
    char input_name [256];
    char output_name[256];

    if ( argc == 1 ) {
        printf("Missing argument\n");
        return EXIT_FAILURE;
    }

    sprintf(input_name , "%s.txt", argv[1]);
    sprintf(output_name, "%s.lp" , argv[1]);

    _csp csp = file_reader(input_name);
    _subproblem_info subproblemInfo;

    if ( argc == 3 ) {
        csp.n_journeys = atoi(argv[2]);
    } else {
        fprintf(stderr, "Assuming the number of journeys is equal to the number of tasks!\n");
        csp.n_journeys = csp.N;
    }

    printf("Starting\n");

    srand(666);

    build_heur_sol ( &csp, subproblemInfo.journeys );

    for (int i = 0; i < (int) subproblemInfo.journeys.size(); ++i) {
        subproblemInfo.usedJourneys[subproblemInfo.journeys[i].covered] = true;
    }

    print_journeys(subproblemInfo.journeys);
    printf("\n");

    return 0;
}
