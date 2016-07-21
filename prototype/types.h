#ifndef TYPES_H
#define TYPES_H

#include <vector>

typedef struct {
    std::vector<int> covered;
    int cost;
} _journey;

typedef struct {
    int id;
    int start_time;
    int end_time;
} _task;

typedef struct {
    int cost;
    int dest;
} _edge;

typedef struct {
    int N;
    int time_limit;
    std::vector<_task> task;
    std::vector<std::vector<_edge > > graph;
    std::vector<int> start_nodes;
    std::vector<int> end_nodes;
} _csp;

#endif /* TYPES_H */
