#ifndef CHVATAL_H
#define CHVATAL_H

#include <vector>
#include "types.h"

void chvatal_heuristic(_csp *csp, std::vector<_journey> &journeys);
void pega_todas_as_jornadas(_csp *csp, _journey *jornada_atual, int no_atual, std::vector<int> tarefas_percorridas, std::vector<int> tarefas_ja_cobertas, double *maior_custo_relativo);
bool pertence(std::vector<int> vetor, int elemento);

#endif /* CHVATAL_H */
