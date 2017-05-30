#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <cassert>
#include <time.h>

#include "types.h"
#include "chvatal.h"

int pos(_csp *csp,int partida,int destino){
	unsigned int i;
	for(i=0;i<csp->graph[partida].size();i++){
		if(csp->graph[partida][i].dest==destino){
			return i;
		}
	}

    assert(0 && "Did not found a position during Chvatal. Aborting!\n");

    return -1;
}

void chvatal_heuristic(_csp *csp, std::vector<_journey> &journeys){
    unsigned int i;
    double menor_custo_relativo;
    int n_tarefas_ja_cobertas = 0;
    std::vector<int> tarefas_ja_cobertas;
    while(csp->N != n_tarefas_ja_cobertas){
    	_journey melhor_jornada;
        melhor_jornada.cost = 0;
        melhor_jornada.time = 0;
    	menor_custo_relativo = -1;

		for(i=0; i<csp->graph.size(); i++){
			std::vector<int> tarefas_percorridas;
			if(!pertence(tarefas_ja_cobertas,i)){
			    pega_todas_as_jornadas(csp, &melhor_jornada,i,tarefas_percorridas,tarefas_ja_cobertas, &menor_custo_relativo);
			}
		}

	    n_tarefas_ja_cobertas += melhor_jornada.covered.size();
	    tarefas_ja_cobertas.push_back(melhor_jornada.covered[0]);
	    for(i=1; i<melhor_jornada.covered.size(); i++){
    		tarefas_ja_cobertas.push_back(melhor_jornada.covered[i]);
	    }
	    journeys.push_back(melhor_jornada);
	}
}

void pega_todas_as_jornadas(_csp *csp, _journey *jornada_atual, int no_atual, std::vector<int> tarefas_percorridas, std::vector<int> tarefas_ja_cobertas, double *menor_custo_relativo){
	unsigned int i;
	tarefas_percorridas.push_back(no_atual);
	_journey jornada_temp;
	jornada_temp.cost = 0;
    jornada_temp.time = 0;
    jornada_temp.covered.push_back(tarefas_percorridas[0]);
	int anterior = tarefas_percorridas[0];
	for(i=1;i<tarefas_percorridas.size();i++){
		jornada_temp.cost += csp->graph[anterior][pos(csp,anterior,tarefas_percorridas[i])].cost;
		anterior = tarefas_percorridas[i];
		jornada_temp.covered.push_back(tarefas_percorridas[i]);
	}
	jornada_temp.time = (csp->task[tarefas_percorridas[tarefas_percorridas.size()-1]].end_time - csp->task[tarefas_percorridas[0]].start_time);
	if(((double)jornada_temp.cost/tarefas_percorridas.size() < *menor_custo_relativo
		&& (double)jornada_temp.cost/tarefas_percorridas.size()!=0)
		|| *menor_custo_relativo == -1
		|| (*menor_custo_relativo == 0 && (double)jornada_temp.cost/tarefas_percorridas.size()!=0)){
		*menor_custo_relativo = (double)jornada_temp.cost/tarefas_percorridas.size();
		jornada_atual->cost = jornada_temp.cost;
	    jornada_atual->time = jornada_temp.time;
	    jornada_atual->covered = jornada_temp.covered;
	}
	for (i=0; i<csp->graph[no_atual].size(); i++){
		if(!pertence(tarefas_percorridas,csp->graph[no_atual][i].dest) && !pertence(tarefas_ja_cobertas,csp->graph[no_atual][i].dest)){
    		if(rand() % 100 < 40){
    			pega_todas_as_jornadas(csp, jornada_atual,csp->graph[no_atual][i].dest,tarefas_percorridas, tarefas_ja_cobertas, menor_custo_relativo);
			}
		}
    }
}

bool pertence(std::vector<int> vetor, int elemento){
	unsigned int i;
	for(i=0;i<vetor.size();i++){
		if(vetor[i]==elemento){
			return true;
		}
	}
	return false;
}

