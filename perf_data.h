#ifndef PERF_DATA_H
#define PERF_DATA_H

typedef struct {
    int executions;
    int good_executions;
    int bad_executions;
    int not_unique_executions;

    double time;
    double good_time;
    double bad_time;
    double not_unique_time;
} _algo_perf_data;

typedef struct {
    _algo_perf_data greedyLp;
    _algo_perf_data greedyHill;
    _algo_perf_data aco;
    _algo_perf_data sa;
    _algo_perf_data tabu;
    _algo_perf_data exact;

    double initial_heur_time;

    double total_time;
    double cplex_time;
    double scip_time;
} _perf_data;

//extern _perf_data perf_data;

void perf_data_init ( _perf_data *perf );
void perf_data_init_algo_data ( _algo_perf_data *p ) ;
void perf_data_show ( _perf_data *p ) ;
void perf_data_save_to_file ( _perf_data *p, char *name ) ;
void perf_data_save_to_file_oneliner ( _perf_data *p, char *name ) ;
_perf_data* get_perf_pointer();

#endif /* PERF_DATA_H */
