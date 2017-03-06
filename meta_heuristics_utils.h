#ifndef META_HEURISTICS_UTILS_H
#define META_HEURISTICS_UTILS_H

#include "types.h"

_journey randomInitialSolution    ( _csp *csp, _subproblem_info *sp                    ) ;

double objectiveFuntion           ( _csp *csp, _subproblem_info *sp, _journey &journey ) ;

void appendTaskToJourneyBeginning ( _csp *csp, _subproblem_info *sp, _journey &journey ) ;
void appendTaskToJourneyEnd       ( _csp *csp, _subproblem_info *sp, _journey &journey ) ;
void removeFirstTaskFromJourney   ( _csp *csp, _subproblem_info *sp, _journey &journey ) ;
void removeLastTaskFromJourney    ( _csp *csp, _subproblem_info *sp, _journey &journey ) ;

#endif /* META_HEURISTICS_UTILS_H */
