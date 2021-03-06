#ifndef BACKTRACK_H
#define BACKTRACK_H

#include <vector>

#include "types.h"

int backtrack                  ( _csp csp, int pos, int cost, int time, int lvl, int sol[], std::vector<_journey> &vec);
int backtrack_biggest_feasible ( _csp csp, int pos, int cost, int time, int lvl, int sol[], std::vector<_journey> &vec);
int backtrack_first_feasible   ( _csp csp, int pos, int cost, int time, int lvl, int sol[], std::vector<_journey> &vec);
int backtrack_all_feasible     ( _csp csp, int pos, int cost, int time, int lvl, int sol[], std::vector<_journey> &vec);

#endif /* BACKTRACK_H */
