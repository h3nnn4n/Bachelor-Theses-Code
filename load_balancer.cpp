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
    //fprintf(stderr, "\nStarting lb\n");
}

int lb_ctrl_get_fit(int i) {
    _perf_data* perf = get_perf_pointer();
    int a = 0;

    switch (i) {
        case 0:
            a = perf->greedyHill.good_executions;
            return a == 0 ? 1 : a;
            break;
        case 1:
            a = perf->aco.good_executions;
            return a == 0 ? 1 : a;
            break;
        case 2:
            a = perf->sa.good_executions;
            return a == 0 ? 1 : a;
            break;
        case 3:
            a = perf->tabu.good_executions;
            return a == 0 ? 1 : a;
            break;
        default:
            fprintf(stderr, "Code shouldnt get here. Aborting!\n Maybe N_METHODS has the wrong value?\n");
            exit(-1);
    }
}

int lb_ctrl_get_next() {
    int total = 0;
    int skip = 0;

    // This gets the total
    for (int i = 0; i < N_METHODS; ++i) {
        if ( lb_controller.used[i] == 0 ) {
            total += lb_ctrl_get_fit(i);
        } else {
            skip += 1;
        }
    }

    double p = drand48();
    double a = 0.0;
    int i = 0;


    while ( 1 ) {
        a += (double)lb_ctrl_get_fit(i) / total;
        //fprintf(stderr, "a = %4.2f p = %4.2f i = %2d\n", a, p, i);

        if ( a >= p ) {
            //fprintf(stderr, "Got a >= p : %2d\n", i);
            lb_controller.used[i] = 1;
            return i;
        } else if ( i >= N_METHODS ) {
            //fprintf(stderr, "Got i >= N_METHODS\n");
            lb_controller.used[i - 1] = 1;
            return i - 1;
        }

        i += 1;
    }
}

