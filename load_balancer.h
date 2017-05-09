#ifndef LOAD_BALANCER_H
#define LOAD_BALANCER_H

#include <vector>
#include "perf_data.h"

#define N_METHODS 4

typedef struct {
    int n;
    int used[N_METHODS];
} _lb_controller;

void lb_ctrl_reset_used();
void lb_ctrl_init();
void lb_ctrl_set_usable( std::vector<bool> &usable);
int lb_ctrl_get_fit(int i);
int lb_ctrl_get_next();

#endif /* LOAD_BALANCER_H */
