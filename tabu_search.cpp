#include <float.h>
#include <limits.h>
#include <math.h>
#include <assert.h>

#include "types.h"
#include "utils.h"

#include "tabu_search.h"

_journey tabuSearch ( _csp *csp, _subproblem_info *sp, double *objValue ) {
    int iter           = 0;
    int max_iter       = 100;
    int tabu_list_size = 15;
    int max_candidates = 50;

    _journey best;
    _journey current;
    _journey bestUnique;
    init_journey(best);
    init_journey(current);
    init_journey(bestUnique);

    best = current;

    do {

    } while ( iter < max_iter );

    exit(0);

    return bestUnique;
}
