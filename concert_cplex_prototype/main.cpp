#include <iostream>
#include <vector>
#include <cstdlib>

#include <map>

#include <ilcplex/cplex.h>
#include <ilcplex/ilocplex.h>

ILOSTLBEGIN

#include "reader.h"
#include "types.h"
#include "printer_to_graphviz.h"
#include "backtrack.h"
#include "utils.h"
#include "random.h"

//#include "check.c"


int main(int argc, char *argv[]) {
    if ( argc != 2 ) {
        printf("Missing argument\n");
        return EXIT_FAILURE;
    }

    double objval_p      = 0;
    int    lp_status     = 0;
    int    non_zero      = 0;
    int    status        = 0;
    int    used_journeys = 0;
    char   *sense        = NULL;
    double *dj           = NULL;
    double *lb           = NULL;
    double *obj          = NULL;
    double *pi           = NULL;
    double *rhs          = NULL;
    double *rmatval      = NULL;
    double *slack        = NULL;
    double *ub           = NULL;
    double *x            = NULL;
    int    *rmatbeg      = NULL;
    int    *rmatcnt      = NULL;
    int    *rmatind      = NULL;

    double *rhs_max      = NULL;
    char   *sense_max    = NULL;
    int    *rmatbeg_max  = NULL;
    int    *rmatind_max  = NULL;
    int    *rmatcnt_max  = NULL;
    double *rmatval_max  = NULL;

    char input_name [256];
    char output_name[256];

    CPXENVptr env = NULL;
    CPXLPptr  lp  = NULL;

    env    = CPXopenCPLEX   ( &status                               ) ;
    status = CPXsetintparam ( env, CPXPARAM_ScreenOutput  , CPX_ON  ) ;
    status = CPXsetintparam ( env, CPXPARAM_Read_DataCheck, CPX_ON  ) ;
    lp     = CPXcreateprob  ( env, &status                , "spp"   ) ;
    status = CPXchgobjsen   ( env, lp                     , CPX_MIN ) ;

    sprintf(input_name , "%s.txt", argv[1]);
    sprintf(output_name, "%s.lp" , argv[1]);

    _csp t = file_reader(input_name);
    std::vector<_journey> journeys;
    std::map<int, bool> covered;

    srand(time(NULL));

#ifdef _GVIZ
    print_to_graphviz(&t);
    return 0;
#endif

    for (int i = 0; i < t.N; ++i)
        covered[i] = false;
    covered[0] = true;

    for (int i = 0; i < (int) ceil(t.N / 3); ++i) {
        _journey p = random_journey(&t);
        journeys.push_back(p);
        for (int j = 0; j < (int) p.covered.size(); ++j) {
            covered[p.covered[j]] = true;
        }
    }

    for (int i = 0; i < (int) covered.size(); ++i) {
        if ( !covered[i] ) {
            std::cout << i << " is not covered" << std::endl;
            if ( test_random_journey_from(&t, i) ) {
                std::cout << "Genearting a cover that starts at it" << std::endl;
                _journey p = random_journey_from(&t, i);
                if ( (int) p.covered.size() < 2 )
                    std::cout << "Something if Funky" << std::endl;
                journeys.push_back(p);
                for (int j = 0; j < (int) p.covered.size(); ++j)
                    covered[p.covered[j]] = true;
            } else {
                std::cout << "It is an end point, generating a cover that has it" << std::endl;
                _journey p = random_journey_to(&t, i);
                if ( (int) p.covered.size() < 2 )
                    std::cout << "Something if Funky" << std::endl;
                journeys.push_back(p);
                for (int j = 0; j < (int) p.covered.size(); ++j)
                    covered[p.covered[j]] = true;
            }
        }
    }

    //int *vec = ( int* ) malloc ( sizeof(int) * t.N );

    //for (int i = 0; i < t.N; ++i)
        //vec[i] = -1;

    //for (int i = 1; i <= t.N; ++i) {
        //backtrack(t, i, 0, 0, 0, vec, journeys);
    //}

    //for (int i = 0; i < (int) t.start_nodes.size(); ++i) {
        //backtrack(t,t.start_nodes[i], 0, 0, vec, journeys);
    //}

    //print_graph(t);
    //printf("\n");
    print_journeys(journeys);
    printf("\n");
    //return 0;

    sense       = ( char  * ) malloc ( sizeof ( char   ) * t.N                      ) ;
    pi          = ( double* ) malloc ( sizeof ( double ) * t.N                      ) ;
    rhs         = ( double* ) malloc ( sizeof ( double ) * t.N                      ) ;
    slack       = ( double* ) malloc ( sizeof ( double ) * t.N                      ) ;

    rmatbeg     = ( int   * ) malloc ( sizeof ( int    ) * (int) journeys.size()    ) ;
    rmatcnt     = ( int   * ) malloc ( sizeof ( int    ) * (int) journeys.size()    ) ;
    dj          = ( double* ) malloc ( sizeof ( double ) * (int) journeys.size()    ) ;
    lb          = ( double* ) malloc ( sizeof ( double ) * (int) journeys.size()    ) ;
    obj         = ( double* ) malloc ( sizeof ( double ) * (int) journeys.size()    ) ;
    ub          = ( double* ) malloc ( sizeof ( double ) * (int) journeys.size()    ) ;
    x           = ( double* ) malloc ( sizeof ( double ) * (int) journeys.size()    ) ;

    rhs_max     = ( double* ) malloc ( sizeof ( double ) * 1                        ) ;
    sense_max   = ( char*   ) malloc ( sizeof ( char   ) * 1                        ) ;
    rmatbeg_max = ( int*    ) malloc ( sizeof ( int    ) * ( int ) journeys.size () ) ;
    rmatind_max = ( int*    ) malloc ( sizeof ( int    ) * ( int ) journeys.size () ) ;
    rmatcnt_max = ( int*    ) malloc ( sizeof ( int    ) * ( int ) journeys.size () ) ;
    rmatval_max = ( double* ) malloc ( sizeof ( double ) * ( int ) journeys.size () ) ;

    for (int i = 0; i < 1; ++i) {
        rhs_max[i]   = 27.0;
        sense_max[i] = 'E';
    }

    for (int i = 0; i < t.N; ++i) {
        rhs  [i] = 1.0;
        sense[i] = 'G';
    }

    for (int i = 0; i < (int) journeys.size(); ++i) { // Contagem de não-zeros
        lb [i] = 0.0;
        ub [i] = 1.0;
        obj[i] = journeys[i].cost;
        rmatbeg_max[i] = i;
        rmatind_max[i] = 0;
        rmatcnt_max[i] = 1;
        rmatval_max[i] = 1;
        for (int j = 0; j < (int) journeys[i].covered.size(); ++j) {
            non_zero++;
        }
    }

    rmatind = (int*   ) malloc (sizeof(int)    * non_zero);
    rmatval = (double*) malloc (sizeof(double) * non_zero);

    for (int i = 0, k = 0; i < (int) journeys.size(); ++i) {  // Cada variavel esta associada a uma jornada
        if ( i ) {
            rmatbeg[i] = (int) journeys[i-1].covered.size();
            rmatbeg[i] += rmatbeg[i-1];
        } else {
            rmatbeg[i] = 0;
        }

        rmatcnt[i] = (int) journeys[i].covered.size();

        for (int j = 0; j < (int) journeys[i].covered.size(); ++j) {
            rmatind[k] = journeys[i].covered[j] - 1;
            rmatval[k] = 1;
            //printf("i:%3d\tk: %3d"
                   //"\t | beg: %2d   ind: %2d   val: %2.1f | "
                   //" -- %5d  %5d %5d\n",
                    //i, k,
                    //rmatbeg[i], rmatind[k], rmatval[k],
                    //rmatbeg[i] + rmatcnt[i], rmatbeg[i], rmatcnt[i]);
            k++;
        }
        //printf("\n"); }
    }

    status = CPXnewrows (env, lp, t.N, rhs, sense, NULL, NULL);
    status = CPXaddcols (env, lp, (int) journeys.size(), non_zero, obj,
                         rmatbeg, rmatind, rmatval,
                         lb, ub, NULL);

    if ( status ) {
        printf("STATUS = %d\nSomething Broke\n", status);
        //checkdata(env, (int) journeys.size(), t.N, CPXgetobjsen(env, lp),
                    //obj, rhs, sense,
                    //rmatbeg, rmatcnt, rmatind, rmatval,
                    //lb, ub,
                    //NULL, NULL, NULL);
        return EXIT_FAILURE;
    }

    //status = CPXaddrows ( env, lp,
                          //0, 1, (int) journeys.size(),
                          //rhs_max, sense_max,
                          //rmatbeg_max, rmatind_max, rmatval_max,
                          //NULL, NULL);

    if ( status ) {
        printf("STATUS = %d\nSomething Broke\n", status);
        //checkdata(env, (int) journeys.size(), 1, CPXgetobjsen(env, lp),
                    //obj, rhs_max, sense_max,
                    //rmatbeg_max, rmatcnt_max, rmatind_max, rmatval_max,
                    //lb, ub,
                    //NULL, NULL, NULL);
        return EXIT_FAILURE;
    }

    status = CPXwriteprob (env, lp, output_name, NULL);
    status = CPXlpopt (env, lp);

    status = CPXsolution (env, lp, &lp_status, &objval_p, x, pi, slack, dj);

    printf("\n");
    printf("N: \t %6d \t maxt: %4d\n", t.N, t.time_limit);
    printf("Found: \t %6d journeys\n", (int) journeys.size());

    for (int i = 0; i < (int) journeys.size(); ++i) {
        if ( x[i] != 0.0 ) {
            used_journeys++;
        }
    }

    printf("Total cost is %6.4f, using %6d journeys\n", objval_p, used_journeys);

    for (int i = 0; i < (int) journeys.size(); ++i) {
        if ( x[i] != 0.0 ) {
            //used_journeys++;
            printf("%6d %3.2f | %6d %6d : ", i, x[i], journeys[i].cost, journeys[i].time);
            for (int j = 0; j < (int) journeys[i].covered.size(); ++j) {
                printf("%3d ", journeys[i].covered[j]);
            }
            printf("\n");
        }
    }

    return EXIT_SUCCESS;
}
