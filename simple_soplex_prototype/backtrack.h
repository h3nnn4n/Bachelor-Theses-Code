#ifndef BACKTRACK_H
#define BACKTRACK_H

#include <vector>

#include "types.h"

int backtrack(_csp csp, int pos, int cost, int lvl, int sol[], std::vector<_journey> &vec);

#endif /* BACKTRACK_H */
