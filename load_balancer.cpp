#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "load_balancer.h"
#include "perf_data.h"

_lb_controller lb_controller;

void lb_ctrl_init() {
    lb_controller.n = N_METHODS;
    for (int i = 0; i < N_METHODS; ++i) {
        lb_controller.used[i] = 0;
    }
}

void lb_ctrl_reset_used() {
    for (int i = 0; i < N_METHODS; ++i) {
        lb_controller.used[i] = 0;
    }
}

void lb_ctrl_set_usable( std::vector<bool> &usable){
    for (int i = 0; i < (int)usable.size(); ++i) {
        if ( usable[i] ) {
            lb_controller.used[i] = 0;
        } else{
            lb_controller.used[i] = 1;
        }
    }
}

int lb_ctrl_get_fit(int i) {
    _perf_data* perf = get_perf_pointer();

    switch (i) {
        case 0:
            return perf->greedyHill.good_executions;
            break;
        case 1:
            return perf->aco.good_executions;
            break;
        case 2:
            return perf->sa.good_executions;
            break;
        case 3:
            return perf->tabu.good_executions;
            break;
        default:
            fprintf(stderr, "Code shouldnt get here. Aborting!\n Maybe N_METHODS has the wrong value?\n");
            exit(-1);
    }
}

int lb_ctrl_get_next() {
    int total = 0;

    // This gets the total
    for (int i = 0; i < N_METHODS; ++i) {
        if ( lb_controller.used[i] == 0 ) {
            total += lb_ctrl_get_fit(i);
        }
    }

    double p = drand48();
    double a = 0.0;
    int i = 0;


    while ( 1 ) {
        a += (double)lb_ctrl_get_fit(i) / total;

        if ( a >= p ) {
            return i;
        } else if ( i >= N_METHODS ) {
            return i - 1;
        }

        i += 1;
    }
}

