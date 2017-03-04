#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <iostream>
#include <vector>
#include <map>

#include "meta_heuristics_utils.h"
#include "simmulated_annealing.h"
#include "types.h"

extern bool debug_sa;

_journey simmulatedAnnealing ( _csp *csp, _subproblem_info *sp, double *objValue ) {
    double t_0   = 50.0;
    double t_n   = 0.0;
    double dt    = 0;
    double temp;
    double energy;
    int iter     = 0;
    int max_iter = 1000;

    //double chanceToRemove = 0.1;
    //double chanceToAppend = 0.1;

    *objValue = 0;
    dt = (t_n - t_0 ) / max_iter;
    temp = t_0;

    _journey solution = randomInitialSolution(csp, sp);
    energy            = objectiveFuntion(csp, sp, solution);

    for (iter = 0; iter < max_iter; ++iter) {
        if ( debug_sa ) printf("\n");
        _journey candidate = solution;

        double p = drand48();

        if(debug_sa)printf("BEGIN CHANGE\n");

        if ( p < 0.1 ) {
            removeFirstTaskFromJourney(csp, sp, candidate);
        } else if ( p < 0.2 ) {
            removeLastTaskFromJourney(csp, sp, candidate);
        } else {
            if ( rand() % 2 == 0 ) {
                //appendTaskToJourneyEnd(csp, sp, candidate);
                appendTaskToJourneyBeginning(csp, sp, candidate);
            } else {
                //appendTaskToJourneyBeginning(csp, sp, candidate);
                appendTaskToJourneyEnd(csp, sp, candidate);
            }
        }
        if(debug_sa)printf("END CHANGE\n");

        double candidateEnergy = objectiveFuntion(csp, sp, candidate);

        //printf("cand %4.2f sol %4.2f  p %4.2f\n", candidateEnergy, energy, exp(-(candidateEnergy - energy)/temp));

        if ( exp(-(candidateEnergy - energy)/temp) >= drand48()) {
            energy   = candidateEnergy;
            solution = candidate;
            if(debug_sa)printf("SWAP\n");
        }

        if(debug_sa){printf(" temp = %4.4f energy = %4.2f cost = %4d time = %4d covered [", temp, energy, solution.cost, solution.time); for (int i = 0; i < (int)solution.covered.size(); ++i) { printf("%4d, ", solution.covered[i]); } printf("\b\b]\n");}

        if ( iter >= 100 && solution.time <= csp->time_limit && sp->usedJourneys.count(solution.covered) == 0 ) {
            //exit(0);
            //*objValue = energy;
            //return solution;
            break;
        }

        temp += dt;
    }

    if ( solution.time < 0 ) {
        exit(0);
    }

    *objValue = energy;
    if ( solution.time > csp->time_limit ) { *objValue = 1; }
    return solution;
}
