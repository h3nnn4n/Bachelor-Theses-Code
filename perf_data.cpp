#include "types.h"
#include "perf_data.h"

void perf_data_init ( _perf_data *perf ) {
    perf_data_init_algo_data ( &perf->greedyLp );
    perf_data_init_algo_data ( &perf->greedyHill );
    perf_data_init_algo_data ( &perf->aco );
    perf_data_init_algo_data ( &perf->sa );
    perf_data_init_algo_data ( &perf->tabu );

    perf->initial_heur_time               = 0;
    perf->total_time                      = 0;
}

void perf_data_init_algo_data ( _algo_perf_data *p ) {
    p->executions            = 0;
    p->good_executions       = 0;
    p->bad_executions        = 0;
    p->not_unique_executions = 0;

    p->time                  = 0;
    p->good_time             = 0;
    p->bad_time              = 0;
    p->not_unique_time       = 0;
}

_perf_data* get_reference () {
    return &perf_data;
}

void perf_data_show ( _perf_data *p ) {
    printf("Time spent generating initial solution: %16.8f\n", p->initial_heur_time);
    printf("Total time spend: %16.8f\n", p->total_time);
}
