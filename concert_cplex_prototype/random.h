#ifndef RANDOM_H
#define RANDOM_H

#include "types.h"

_journey random_journey       ( _csp *csp                 ) ;
_journey random_journey_from  ( _csp *csp, int start_from ) ;
_journey random_journey_to    ( _csp *csp, int start_from ) ;
bool test_random_journey_from ( _csp *csp, int start_from ) ;

#endif /* RANDOM_H */
