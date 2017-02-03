#ifndef UTILS_H
#define UTILS_H

#include "types.h"
#include <vector>

void print_journeys(std::vector<_journey> &journeys);
void print_graph(_csp csp);
void init_journey( _journey &journey);

#endif /* UTILS_H */
