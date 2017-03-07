#include "time_keeper.h"

#include <time.h>
#include <unistd.h>

struct timespec time_diff(struct timespec start, struct timespec end) {
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

void timekeeper_tic  (struct timespec *t1) {
    clock_gettime(CLOCK_MONOTONIC_RAW, t1);
}

double time_diff_double(struct timespec start, struct timespec end) {
    return time_diff(start, end).tv_sec + 1e-9 * time_diff(start, end).tv_nsec;
}
