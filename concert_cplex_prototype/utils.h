#ifndef UTILS_H
#define UTILS_H

#include <vector>

#include "types.h"

void print_journeys  ( std::vector<_journey> &journeys         ) ;
void print_graph     ( _csp csp                                ) ;
void init_journey    ( _journey &journey                       ) ;
void validateJourney ( _subproblem_info *sp, _journey &journey ) ;
float calculateJourneyReducedCost ( _subproblem_info *sp, _journey &journey) ;

#endif /* UTILS_H */
