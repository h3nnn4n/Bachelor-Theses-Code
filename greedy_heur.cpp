#include "greedy_heur.h"
#include "types.h"

#include <ilcplex/ilocplex.h>
#include <ilcp/cpext.h>

bool debug_HillClimbing = false;
bool debig_GreedyLp = false;

_journey greedyHillClimbingHeur ( _csp *csp, _subproblem_info *sp, double *objValue ) {
    if(debug_HillClimbing)printf("Running greedyHillClimbingHeur\n");
    _journey journey;

    for (int i = 0; i < csp->N; ++i) {
        int atual = i;
        double obj = - sp->mi - sp->duals[atual];

        journey.time = 0;
        journey.cost = 0;
        journey.covered.clear();
        journey.time += sp->time_mat[csp->N][atual];
        journey.covered.push_back(atual);

        assert ( journey.time > 0 && "JOURNEY TIME <= ZERO!!");

        bool found_something;
        do {
            found_something = false;
            for (int j = 0; j < (int) csp->graph[atual].size(); ++j) {
                int dest = csp->graph[atual][j].dest;
                if ( sp->cost_mat[atual][dest] + obj < 0 ) {
                    if ( sp->time_mat[atual][dest] + journey.time <= csp->time_limit ) {
                        journey.cost += sp->cost_mat[atual][dest];
                        journey.time += sp->time_mat[atual][dest];
                        journey.covered.push_back(dest);
                        found_something = true;
                        if(debug_HillClimbing)printf("%3d -> %3d\n", atual, dest);
                        atual = dest;
                    }
                }
            }
        } while ( found_something );

        if ( obj <= 0 && journey.covered.size() > 1 &&  sp->usedJourneys.count(journey.covered) == 0 ) {
            if(debug_HillClimbing)printf("Found new journey with reduced cost < 0\n");
            *objValue = obj;
            break;
        }
    }


    if(debug_HillClimbing)printf("Reduced value = %4.8f\n", *objValue);

    return journey;
}

_journey greedyLpHeur ( _csp *csp, _subproblem_info *sp, IloArray<IloNumVarArray> y, IloEnv &env, IloCplex &cplex_final, double *objValue ) {
    if(debig_GreedyLp)printf("Running greedyLpheur\n");
    _journey journey;
    journey.time = 0;
    journey.cost = 0;
    journey.covered.clear();

    IloNumArray vals(env);

    double best = 0;
    int bestIndex = -1;
    int doit = 1;
    int atual = csp->N;

    *objValue = 0;

    *objValue -= sp->mi; // Mi

    do {
        cplex_final.getValues(vals, y[atual]);

        for (int i = 0; i < csp->N+2; ++i) {
            if ( vals[i] > best ) {
                bestIndex = i;
                best = vals[i];
            }
        }

        if ( bestIndex != -1 && best > 0 && bestIndex <= csp->N ) {
            int j = bestIndex;

            if ( journey.time + sp->time_mat[atual][j] < csp->time_limit ) {
                journey.cost += sp->cost_mat[atual][j];
                journey.time += sp->time_mat[atual][j];

                *objValue += sp->cost_mat[atual][j];

                //printf("Covering %3d -> %3d = %4.8f  cost = %4.4f time = %4.4f dual = %4.4f\n", atual, j, vals[j], (float)cost_mat[atual][j], (float)sp->time_mat[atual][j], duals[j] );
                if(debig_GreedyLp)printf("Covering %3d -> %3d", atual, j);
                if(debig_GreedyLp)printf(" = %4.8f  cost = %4.4f time = %4.4f dual = %4.4f\n", vals[j], (float)sp->cost_mat[atual][j], (float)sp->time_mat[atual][j], sp->duals[j] );

                atual = bestIndex;
                best  = 0;

                *objValue -= sp->duals[atual];

                journey.covered.push_back ( atual );
            } else {
                if(debig_GreedyLp)printf("MaxTime limit %4.8f, stoping\n", (float)journey.time);
                doit = false;
            }
        } else {
            //assert ( 0 && "This should never happen" );
            if(debig_GreedyLp)printf("Found the last task with time %4.8f, stoping\n", (float)journey.time);
            doit = false;
        }
    } while ( doit && atual < csp->N+1 );

    if(debig_GreedyLp)printf("Reduced value = %4.8f\n", *objValue);

    return journey;
}


