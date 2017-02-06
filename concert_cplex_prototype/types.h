#ifndef TtYPES_H
#define TYPES_H

#include <vector>
#include <map>

typedef struct {
    std::vector<int> covered;
    int cost;
    int time;
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
    int N;   // Number of tasks
    int n_journeys; // Expected number of journeys
    int time_limit; //
    std::vector<_task> task;
    std::vector<std::vector<_edge > > graph;
    std::vector<int> start_nodes;
    std::vector<int> end_nodes;
} _csp;

typedef struct {
    double **adj_mat;
    double **cost_mat;
    double **time_mat;
    double *duals;
    double mi;

    int N;

    std::vector<_journey> journeys;
    std::map<std::vector<int>, bool> usedJourneys;
} _subproblem_info;

#endif /* TYPES_H */
