#include <iostream>
#include <vector>
#include <cstdlib>

#include <ilcplex/cplex.h>

#include "reader.h"
#include "types.h"
#include "printer_to_graphviz.h"
#include "backtrack.h"
#include "utils.h"

#include "check.c"

int main(int argc, char *argv[]) {
    if ( argc != 2 ) {
        printf("Missing argument\n");
        return EXIT_FAILURE;
    }

    int    status;
    //int    used_journeys = 0;
    int    non_zero      = 0;
    double *obj          = NULL;
    double *lb           = NULL;
    double *ub           = NULL;
    double *rhs          = NULL;
    char   *sense        = NULL;
    int    *rmatbeg      = NULL;
    int    *rmatind      = NULL;
    int    *rmatcnt      = NULL;
    double *rmatval      = NULL;
    int    zeroind_p     = 0;
    double minval_p      = 0;
    double maxval_p      = 0;

    char input_name[256];
    char output_name[256];

    CPXENVptr     env = NULL;
    CPXLPptr      lp = NULL;

    env    = CPXopenCPLEX   ( &status                               ) ;
    status = CPXsetintparam ( env, CPXPARAM_ScreenOutput  , CPX_ON  ) ;
    status = CPXsetintparam ( env, CPXPARAM_Read_DataCheck, CPX_ON  ) ;
    lp     = CPXcreateprob  ( env, &status                , "spp"   ) ;
    status = CPXchgobjsen   ( env, lp                     , CPX_MIN ) ;

    sprintf(input_name , "%s.txt", argv[1]);
    sprintf(output_name, "%s.lp" , argv[1]);

    _csp t = file_reader(input_name);
    std::vector<_journey> journeys;

    //print_to_graphviz(&t);
    //return 0;

    int *vec = ( int* ) malloc ( sizeof(int) * t.N );

    for (int i = 0; i < t.N; ++i) {
        vec[i] = -1;
    }

    for (int i = 1; i <= t.N; ++i) {
        backtrack(t, i, 0, 0, vec, journeys);
    }

    //print_graph(t);
    //printf("\n");
    //print_journeys(journeys);
    //printf("\n");
    //return 0;

    rhs     = (double*) malloc ( sizeof(double) * t.N                   );
    obj     = (double*) malloc ( sizeof(double) * (int) journeys.size() );
    lb      = (double*) malloc ( sizeof(double) * (int) journeys.size() );
    ub      = (double*) malloc ( sizeof(double) * (int) journeys.size() );
    sense   = (char  *) malloc ( sizeof(char  ) * t.N                   );
    rmatbeg = (int   *) malloc ( sizeof(int   ) * (int) journeys.size() );
    rmatcnt = (int   *) malloc ( sizeof(int   ) * (int) journeys.size() );

    for (int i = 0; i < t.N; ++i) {
        rhs  [i] = 1.0;
        sense[i] = 'E';
    }

    for (int i = 0; i < (int) journeys.size(); ++i) { // Contagem de não-zeros
        lb [i] = 0.0;
        ub [i] = 1.0;
        obj[i] = journeys[i].cost;
        for (int j = 0; j < (int) journeys[i].covered.size(); ++j) {
            non_zero++;
        }
    }

    rmatind = (int*   ) malloc (sizeof(int)    * non_zero);
    rmatval = (double*) malloc (sizeof(double) * non_zero);

    for (int i = 0, k = 0; i < (int) journeys.size(); ++i) {  // Cada variavel esta associada a uma jornada
        if ( i ) {
            rmatbeg[i] = (int) journeys[i].covered.size();
            rmatbeg[i] += rmatbeg[i-1];
        } else {
            rmatbeg[i] = 0;
        }

        rmatcnt[i] = (int) journeys[i].covered.size();

        for (int j = 0; j < (int) journeys[i].covered.size(); ++j) {
            rmatind[k] = j;
            rmatval[k] = 1;
            printf("i:%3d\tk: %3d \t%5d\t%5d\t%5.2f\n", i, k, rmatbeg[i], rmatind[k], rmatval[k]);
            k++;
        }
        printf("\n");
    }

    //status = CPXnewcols (env, lp, (int) journeys.size(), obj, lb, ub, NULL, NULL);
    status = CPXnewrows (env, lp, t.N, rhs, sense, NULL, NULL);
    status = CPXaddcols (env, lp, (int) journeys.size(), non_zero, obj,
                         rmatbeg, rmatind, rmatval,
                         lb, ub, NULL);

    if ( status ) {
        printf("STATUS = %d\n", status);

        printf("Checking\n");
        //CPXCHANNELptr  *c1 = NULL;
        //CPXCHANNELptr  *c2 = NULL;
        //status = CPXaddfuncdest (env, c1, "error", ourmsgfunc);
        //CPXgetchannels (env, NULL, c1, c2, NULL);
        checkmatval(env, rmatbeg, rmatcnt, rmatval, (int) journeys.size(), NULL, NULL, NULL);
        checkcols(env, (int) journeys.size(), t.N, obj, rmatbeg, rmatcnt, rmatind, rmatval, lb, lb,
                &zeroind_p, &minval_p, &maxval_p, NULL, NULL, NULL);
        printf("Checked\n");

        return EXIT_FAILURE;
    }

    status = CPXlpopt (env, lp);
    status = CPXwriteprob (env, lp, output_name, NULL);

    printf("\n");
    printf("N: \t %6d \t maxt: %4d\n", t.N, t.time_limit);
    printf("Found: \t %6d journeys\n", (int) journeys.size());

    //if( stat == soplex::SPxSolver::OPTIMAL )
    //{
        //mysoplex.getPrimalReal(prim);
        //mysoplex.getDualReal(dual);
        //std::cout << "LP solved to optimality.\n";
        //std::cout << "Objective value is " << mysoplex.objValueReal() << ".\n";
        //std::cout << "Primal solution is [";
        //for (int i = 0; i < (int) journeys.size() - 1; ++i) {
            //printf("%.1f, ", prim[i]);
        //}
        //printf("%.1f ]\n", prim[(int) journeys.size() - 1]);

        //std::cout << "Dual solution is [";
        //for (int i = 0; i < t.N - 1; ++i) {
            //printf("%.1f, ", dual[i]);
        //}
        //printf("%.1f ]\n", dual[t.N - 1]);
        ////for (int i = 0; i < t.N; ++i) {
            ////std::cout << dual[i] << ", ";
        ////}
        ////std::cout << "].\n";

        //printf("Problem has\n%d columns\n%d rows\n", mysoplex.numColsReal(), mysoplex.numRowsReal());
        //printf("Found %d possible journeys\n", (int) journeys.size());
        //printf("Solution is:\n");

        //for (int i = 0; i < (int) journeys.size(); ++i) {
            //if ( prim[i] > 0.0 ) {
                //used_journeys++;
                //printf("%4d %.2f : ", i, prim[i]);
                //for (int j = 0; j < (int) journeys[i].covered.size(); ++j) {
                    //printf("%4d ", journeys[i].covered[j]);
                //}
                //printf("\n");
            //}
        //}
        //printf("Using %d journeys\n", used_journeys);
    //}


    return 0;
}
