#include <iostream>
#include <vector>
#include <cstdlib>

#include <glpk.h>

#include "reader.h"
#include "types.h"
#include "printer_to_graphviz.h"
#include "backtrack.h"
#include "utils.h"

int main(void) {
    glp_prob *lp;
    glp_iocp parm_mip;
    glp_smcp parm_spx;

    bool max_journey = false;
    int total_journeys = 0;

    std::vector<int>    ia;
    std::vector<int>    ja;
    std::vector<double> ar;

    ia.push_back(0);
    ja.push_back(0);
    ar.push_back(0);

    _csp t = file_reader("csp5.txt");
    std::vector<_journey> journeys;

    //print_to_graphviz(&t);
    //return 0;

    int *vec = ( int* ) malloc ( sizeof(int) * t.N );
    //for (int i = 0; i < (int)t.start_nodes.size(); ++i)
    //std::cout << t.N << '\n';

    for (int i = 0; i < t.N; ++i) {
        vec[i] = -1;
    }

    for (int i = 1; i <= t.N; ++i) {
        backtrack(t, i, 0, 0, vec, journeys);
    }

    print_graph(t);
    printf("\n");
    print_journeys(journeys);
    printf("\n");
    //return 0;

    lp = glp_create_prob();
    glp_set_prob_name(lp, "tsp");
    glp_set_obj_dir(lp, GLP_MIN);

    glp_init_iocp(&parm_mip);

    parm_mip.msg_lev    = GLP_MSG_OFF;      // Output level

    glp_init_smcp(&parm_spx);

    parm_spx.msg_lev = GLP_MSG_ALL;

    glp_add_cols(lp, (int) journeys.size());

    for (int i = 1; i <= (int) journeys.size(); ++i) {  // Cada variavel esta associada a uma jornada
        glp_set_col_bnds(lp, i, GLP_DB, 0.0, 1.0);     // 0 <= x_j <= 1 \forall j \in J
        glp_set_col_kind(lp, i, GLP_IV);               // x_j é inteiro
        glp_set_obj_coef(lp, i, journeys[i].cost);
    }

    for (int i = 0; i < t.N; ++i) {
        int new_row = glp_add_rows(lp, 1);
        glp_set_row_bnds(lp, new_row, GLP_FX, 1.0, 1.0); // Cada tarefa deve ser coberta exatamente uma vez
    }

    printf("\n----------------\n");
    for (int i = 0; i < (int) journeys.size(); ++i) {  // Cada variavel esta associada a uma jornada
        for (int j = 0; j < (int) journeys[i].covered.size(); ++j) {
            ia.push_back( journeys[i].covered[j] );
            ja.push_back( i+1 );
            ar.push_back( 1   );
            std::cout << journeys[i].covered[j] << " " << i << "\n";
        }
    }
    printf("\n----------------\n");

    for (int i = 0; i < (int) ia.size(); ++i) {
        //printf("%d %d %.2f\n", ia[i], ja[i], ar[i]);
    }


    if ( max_journey ) {
        int new_row = glp_add_rows(lp, 1);
        for (int i = 1; i <= t.N; ++i) {
            ia.push_back(new_row );
            ja.push_back(i       );
            ar.push_back(1       );
        }
        glp_set_row_bnds(lp, t.N, GLP_DB, 04.0, 30.0); // Necessita de exatamente X jornadas
    }

    glp_load_matrix(lp, (int) ia.size()-1, &ia[0], &ja[0], &ar[0]);

    glp_simplex(lp, &parm_spx);

    for (int i = 1; i <= (int) journeys.size(); ++i) {
        if ( glp_get_col_prim(lp, i) > 0.001 ) {
            total_journeys++;
        }
    }

    printf("Problem has\n%d columns\n%d rows\n", (int)journeys.size(), t.N);
    printf("Found %d possible journeys\n", (int) journeys.size());
    printf("Optimal solution is: %.3f with %d journeys\n", glp_get_obj_val(lp), total_journeys);

    for (int i = 1; i <= (int) journeys.size(); ++i) {
        if ( glp_get_col_prim(lp, i) > 0.001 ) {
            printf("%d %.2f: ", i, glp_get_col_prim(lp, i));
            for (int j = 0; j < (int) journeys[i-1].covered.size(); ++j) {
                printf("%d ", journeys[i-1].covered[j]);
            }
            printf("\n");
        }
        //printf("%.3f ", glp_get_col_prim(lp, i));
    }
    printf("\n");

    //std::cout << "Found " << glp_get_obj_val(lp) << " tasks\n";
    return 0;
}
