#include <float.h>
#include <limits.h>
#include <math.h>
#include <assert.h>

#include <queue>
#include <map>

#include "types.h"
#include "utils.h"

#include "meta_heuristics_utils.h"
#include "tabu_search.h"

_journey tabuSearch ( _csp *csp, _subproblem_info *sp, double *objValue ) {
    int iter           = 0;
    int max_iter       = 100;
    int tabu_list_size = 15;
    int max_candidates = 5;

    float bestCost       = FLT_MAX;
    float bestUniqueCost = FLT_MAX;

    std::map<std::vector<int>, bool> isTabu;
    std::queue<std::vector<int>>     tabuList;

    bool first = true;
    int restartCounter = 0;
    float oldBest = 0;

    _journey best;
    _journey current;
    _journey bestUnique;
    init_journey(best);
    init_journey(current);
    init_journey(bestUnique);

    current = randomInitialSolution(csp, sp);

    best = current;
    bestCost = calculateJourneyReducedCost(sp, best);

    do {
        for (int i = 0; i < max_candidates; ++i) {
            double p = drand48();
            _journey candidate = best;

            if ( restartCounter > 10 ) {
                //printf("Restart\n");
                candidate = randomInitialSolution(csp, sp);
            }

            if ( candidate.time > csp->time_limit ) {
                if ( p < 0.5 ) {
                    removeFirstTaskFromJourney(csp, sp, candidate);
                } else {
                    removeLastTaskFromJourney(csp, sp, candidate);
                }
            } else {
                if ( p < 0.1 ) {
                    removeFirstTaskFromJourney(csp, sp, candidate);
                } else if ( p < 0.2 ) {
                    removeLastTaskFromJourney(csp, sp, candidate);
                } else {
                    if ( rand() % 2 == 0 ) {
                        appendTaskToJourneyBeginning(csp, sp, candidate);
                    } else {
                        appendTaskToJourneyEnd(csp, sp, candidate);
                    }
                }
            }

            if ( !first ) {
                if ( oldBest == bestCost ) {
                    restartCounter ++;
                } else {
                    restartCounter = 0;
                }

                oldBest = bestCost;
            }

            if ( first || ( calculateJourneyReducedCost(sp, candidate) < bestCost &&
                          (isTabu.count(candidate.covered) == 0 || isTabu[candidate.covered] == false ))) {
                if ( first ) first = false;

                if ( candidate.time <= csp->time_limit ) {
                    best     = candidate;
                    bestCost = calculateJourneyReducedCost(sp, candidate);
                }
            }

            if ( ( calculateJourneyReducedCost(sp, candidate) < bestUniqueCost &&
                 (isTabu.count(candidate.covered) == 0 || isTabu[candidate.covered] == false ))) {

                if ( candidate.time <= csp->time_limit ) {
                    bestUniqueCost = calculateJourneyReducedCost(sp, candidate );
                    bestUnique     = candidate;
                }
            }
        }

        if ((isTabu.count(best.covered) == 0 || isTabu[best.covered] == false )) {
            tabuList.push(best.covered);
            isTabu[best.covered] = true;
        }

        if ( (int)tabuList.size() > tabu_list_size ) {
            isTabu[tabuList.front()] = false;
            tabuList.pop();
        }

        //printf("%4.4f %4.4f\n", bestCost, bestUniqueCost);

        iter++;

        if ( bestUniqueCost < 0 && bestUnique.time <= csp->time_limit ) {
            break;
        }
    } while ( iter < max_iter );

    //exit(0);

    *objValue = bestUniqueCost;
    if ( bestUnique.time > csp->time_limit ) { *objValue = 1; }

    return bestUnique;
}
