#include "types.h"
#include "perf_data.h"

static _perf_data perf_data;

_perf_data* get_perf_pointer(){
    return &perf_data;
}

void perf_data_init ( _perf_data *perf ) {
    perf_data_init_algo_data ( &perf->greedyLp );
    perf_data_init_algo_data ( &perf->greedyHill );
    perf_data_init_algo_data ( &perf->aco );
    perf_data_init_algo_data ( &perf->sa );
    perf_data_init_algo_data ( &perf->tabu );
    perf_data_init_algo_data ( &perf->exact );

    perf->initial_heur_time               = 0;
    perf->total_time                      = 0;
    perf->cplex_time                      = 0;
    perf->scip_time                       = 0;
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

void perf_data_save_to_file_oneliner ( _perf_data *p, char *name ) {
    FILE *f = fopen(name, "at");

    _algo_perf_data *a;
    fprintf(f, "%16.8f", p->initial_heur_time       ) ;
    fprintf(f, "%16.8f", p->total_time              ) ;
    fprintf(f, "%16.8f", p->cplex_time              ) ;
    fprintf(f, "%16.8f", p->scip_time               ) ;

    a = &p->greedyHill;
    fprintf(f, "%8d %8.8f", a->executions           , a->time              ) ;
    fprintf(f, "%8d %8.8f", a->good_executions      , a->good_time         ) ;
    fprintf(f, "%8d %8.8f", a->bad_executions       , a->bad_time          ) ;
    fprintf(f, "%8d %8.8f", a->not_unique_executions, a->not_unique_time   ) ;

    a = &p->sa;
    fprintf(f, "%8d %8.8f", a->executions           , a->time              ) ;
    fprintf(f, "%8d %8.8f", a->good_executions      , a->good_time         ) ;
    fprintf(f, "%8d %8.8f", a->bad_executions       , a->bad_time          ) ;
    fprintf(f, "%8d %8.8f", a->not_unique_executions, a->not_unique_time   ) ;

    a = &p->aco;
    fprintf(f, "%8d %8.8f", a->executions           , a->time              ) ;
    fprintf(f, "%8d %8.8f", a->good_executions      , a->good_time         ) ;
    fprintf(f, "%8d %8.8f", a->bad_executions       , a->bad_time          ) ;
    fprintf(f, "%8d %8.8f", a->not_unique_executions, a->not_unique_time   ) ;

    a = &p->tabu;
    fprintf(f, "%8d %8.8f", a->executions           , a->time              ) ;
    fprintf(f, "%8d %8.8f", a->good_executions      , a->good_time         ) ;
    fprintf(f, "%8d %8.8f", a->bad_executions       , a->bad_time          ) ;
    fprintf(f, "%8d %8.8f", a->not_unique_executions, a->not_unique_time   ) ;

    a = &p->exact;
    fprintf(f, "%8d %8.8f", a->executions           , a->time              ) ;
    fprintf(f, "%8d %8.8f", a->good_executions      , a->good_time         ) ;
    fprintf(f, "%8d %8.8f", a->bad_executions       , a->bad_time          ) ;
    fprintf(f, "%8d %8.8f", a->not_unique_executions, a->not_unique_time   ) ;

    fprintf(f, "\n");

    fclose(f);
}

void perf_data_save_to_file ( _perf_data *p, char *name ) {
    FILE *f = fopen(name, "wt");

    _algo_perf_data *a;
    fprintf(f, "Time spent generating initial solution: %16.8f\n", p->initial_heur_time       ) ;
    fprintf(f, "Total time spent:                       %16.8f\n", p->total_time              ) ;
    fprintf(f, "Total time spent on cplex:              %16.8f\n", p->cplex_time              ) ;
    fprintf(f, "Total time spent on scip:               %16.8f\n", p->scip_time               ) ;

    fprintf(f, "\n"                                                                           ) ;
    fprintf(f, "HillClimbing:\n"                                                              ) ;
    a = &p->greedyHill;
    fprintf(f, "Total runs:      %4d %4.4f\n", a->executions           , a->time              ) ;
    fprintf(f, "Good runs:       %4d %4.4f\n", a->good_executions      , a->good_time         ) ;
    fprintf(f, "Bad runs:        %4d %4.4f\n", a->bad_executions       , a->bad_time          ) ;
    fprintf(f, "Not unique runs: %4d %4.4f\n", a->not_unique_executions, a->not_unique_time   ) ;

    fprintf(f, "\n"                                                                           ) ;
    fprintf(f, "Simulated Annealing:\n"                                                       ) ;
    a = &p->sa;
    fprintf(f, "Total runs:      %4d %4.4f\n", a->executions           , a->time              ) ;
    fprintf(f, "Good runs:       %4d %4.4f\n", a->good_executions      , a->good_time         ) ;
    fprintf(f, "Bad runs:        %4d %4.4f\n", a->bad_executions       , a->bad_time          ) ;
    fprintf(f, "Not unique runs: %4d %4.4f\n", a->not_unique_executions, a->not_unique_time   ) ;

    fprintf(f, "\n"                                                                           ) ;
    fprintf(f, "Ant Colony Optimization:\n"                                                   ) ;
    a = &p->aco;
    fprintf(f, "Total runs:      %4d %4.4f\n", a->executions           , a->time              ) ;
    fprintf(f, "Good runs:       %4d %4.4f\n", a->good_executions      , a->good_time         ) ;
    fprintf(f, "Bad runs:        %4d %4.4f\n", a->bad_executions       , a->bad_time          ) ;
    fprintf(f, "Not unique runs: %4d %4.4f\n", a->not_unique_executions, a->not_unique_time   ) ;

    fprintf(f, "\n"                                                                           ) ;
    fprintf(f, "Tabu Search:\n"                                                               ) ;
    a = &p->tabu;
    fprintf(f, "Total runs:      %4d %4.4f\n", a->executions           , a->time              ) ;
    fprintf(f, "Good runs:       %4d %4.4f\n", a->good_executions      , a->good_time         ) ;
    fprintf(f, "Bad runs:        %4d %4.4f\n", a->bad_executions       , a->bad_time          ) ;
    fprintf(f, "Not unique runs: %4d %4.4f\n", a->not_unique_executions, a->not_unique_time   ) ;

    fprintf(f, "\n"                                                                           ) ;
    fprintf(f, "Exact:\n"                                                                     ) ;
    a = &p->exact;
    fprintf(f, "Total runs:      %4d %4.4f\n", a->executions           , a->time              ) ;
    fprintf(f, "Good runs:       %4d %4.4f\n", a->good_executions      , a->good_time         ) ;
    fprintf(f, "Bad runs:        %4d %4.4f\n", a->bad_executions       , a->bad_time          ) ;
    fprintf(f, "Not unique runs: %4d %4.4f\n", a->not_unique_executions, a->not_unique_time   ) ;

    fclose(f);
}

void perf_data_show ( _perf_data *p ) {
    _algo_perf_data *a;
    fprintf(stderr, "Time spent generating initial solution: %16.8f\n", p->initial_heur_time       ) ;
    fprintf(stderr, "Total time spent:                       %16.8f\n", p->total_time              ) ;
    fprintf(stderr, "Total time spent on cplex:              %16.8f\n", p->cplex_time              ) ;
    fprintf(stderr, "Total time spent on scip:               %16.8f\n", p->scip_time               ) ;

    fprintf(stderr, "\n"                                                                           ) ;
    fprintf(stderr, "HillClimbing:\n"                                                              ) ;
    a = &p->greedyHill;
    fprintf(stderr, "Total runs:      %4d %4.4f\n", a->executions           , a->time              ) ;
    fprintf(stderr, "Good runs:       %4d %4.4f\n", a->good_executions      , a->good_time         ) ;
    fprintf(stderr, "Bad runs:        %4d %4.4f\n", a->bad_executions       , a->bad_time          ) ;
    fprintf(stderr, "Not unique runs: %4d %4.4f\n", a->not_unique_executions, a->not_unique_time   ) ;

    fprintf(stderr, "\n"                                                                           ) ;
    fprintf(stderr, "Simulated Annealing:\n"                                                       ) ;
    a = &p->sa;
    fprintf(stderr, "Total runs:      %4d %4.4f\n", a->executions           , a->time              ) ;
    fprintf(stderr, "Good runs:       %4d %4.4f\n", a->good_executions      , a->good_time         ) ;
    fprintf(stderr, "Bad runs:        %4d %4.4f\n", a->bad_executions       , a->bad_time          ) ;
    fprintf(stderr, "Not unique runs: %4d %4.4f\n", a->not_unique_executions, a->not_unique_time   ) ;

    fprintf(stderr, "\n"                                                                           ) ;
    fprintf(stderr, "Ant Colony Optimization:\n"                                                   ) ;
    a = &p->aco;
    fprintf(stderr, "Total runs:      %4d %4.4f\n", a->executions           , a->time              ) ;
    fprintf(stderr, "Good runs:       %4d %4.4f\n", a->good_executions      , a->good_time         ) ;
    fprintf(stderr, "Bad runs:        %4d %4.4f\n", a->bad_executions       , a->bad_time          ) ;
    fprintf(stderr, "Not unique runs: %4d %4.4f\n", a->not_unique_executions, a->not_unique_time   ) ;

    fprintf(stderr, "\n"                                                                           ) ;
    fprintf(stderr, "Tabu Search:\n"                                                               ) ;
    a = &p->tabu;
    fprintf(stderr, "Total runs:      %4d %4.4f\n", a->executions           , a->time              ) ;
    fprintf(stderr, "Good runs:       %4d %4.4f\n", a->good_executions      , a->good_time         ) ;
    fprintf(stderr, "Bad runs:        %4d %4.4f\n", a->bad_executions       , a->bad_time          ) ;
    fprintf(stderr, "Not unique runs: %4d %4.4f\n", a->not_unique_executions, a->not_unique_time   ) ;

    fprintf(stderr, "\n"                                                                           ) ;
    fprintf(stderr, "Exact:\n"                                                                     ) ;
    a = &p->exact;
    fprintf(stderr, "Total runs:      %4d %4.4f\n", a->executions           , a->time              ) ;
    fprintf(stderr, "Good runs:       %4d %4.4f\n", a->good_executions      , a->good_time         ) ;
    fprintf(stderr, "Bad runs:        %4d %4.4f\n", a->bad_executions       , a->bad_time          ) ;
    fprintf(stderr, "Not unique runs: %4d %4.4f\n", a->not_unique_executions, a->not_unique_time   ) ;
}
