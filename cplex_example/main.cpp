/*
 * O problema eh:
 *
 * Max 2x1 + 4x2
 * S.T.
 *   x1 - 3x2 <=  1
 *  2x1 + 2x2 <= 10
 *   x1 +  x2 >=  1
 *  3x1 +  x2  =  3
 *  0 <= x1, x2 <= 50
 *
 *  então:
 *
 * A = 1 -3        // Matriz de restricoes
 *     2  2
 *     1  1
 *     3  1
 *
 * b = 1 10 1 3    // Lado direito (RHS)
 * c = 2 4         // Funcao objetivo
 *
 * O modelo eh:
 * Max cx
 * S.T.
 *   Ax = b
 *
 */

#include <iostream>
#include <vector>
#include <cstdlib>

#include <ilcplex/cplex.h>

#include "/opt/ibm/ILOG/CPLEX_Studio1263/cplex/examples/src/c/check.c"

#define  COLSORIG  2
#define  ROWSSUB   4
#define  NZSUB     (2*COLSORIG)
#define  ROWSCOMP  4
#define  NZCOMP    (ROWSCOMP*COLSORIG)
#define  ROWSTOT   (ROWSSUB+ROWSCOMP)
#define  NZTOT     (NZCOMP+NZSUB)

int main( void ) {
   double     Arhs   [ROWSTOT ] = { 1  , 10 , 1  , 3 };   // Lado direito (vetor b)
   double     Alb    [COLSORIG] = { 0  , 0  };            // Limite superior e inferior das variaveis
   double     Aub    [COLSORIG] = { 50 , 50 };            // Limite superior e inferior das variaveis
   double     Acost  [COLSORIG] = { 2, 4 };               // Coeficientes da funcao obj (vetor c)
   char       Asense [ROWSTOT ] = { 'L', 'L', 'G', 'E' }; // Tipos das restriçoes: L eh <=, G eh >=, E eh =
   int        Amatbeg[COLSORIG] = { 0, 4 };               // Quantos não zeros tem na coluna anterior
   int        Amatcnt[COLSORIG] = { 4, 4 };               // Quantos nao zeros tem na coluna atual
   int        Amatind[NZTOT   ] = { 0, 1, 2, 3,           // Os indicices de cada coluna que eh diferente de zero
                                    0, 1, 2, 3, };
   double     Amatval[NZTOT   ] = { 1, 2,                 // Matriz A, aqui ela eh tratada como um vetor, vc soh coloca o que for diferente de zero
                                    1, 3,
                                   -3, 2,
                                    1, 1 };               // Essa codificacao eh bem bizzara, se nao entendeu me pergunta

    CPXENVptr env = NULL;
    CPXLPptr  lp = NULL;

    int       j;
    int       status, lpstat;
    double    objval;
    double    x[COLSORIG];


    env    = CPXopenCPLEX   ( &status                                           ) ; // Isso aqui carrega o cplex
    status = CPXsetintparam ( env, CPXPARAM_ScreenOutput, CPX_ON                ) ; // Qnta informacao o solver mostra na tela
    status = CPXsetintparam ( env, CPXPARAM_Simplex_Display, 2                  ) ; // mesma coisa ^
    lp     = CPXcreateprob  ( env, &status, "huehuehuebrbr"                     ) ; // Isso aqui cria um problema vazio
    status = CPXcopylp      ( env, lp, COLSORIG, ROWSSUB, CPX_MIN, Acost, Arhs,     // Aqui eh carregado aquele monte de vetor das linhas 46-58
                              Asense, Amatbeg, Amatcnt, Amatind, Amatval,
                              Alb, Aub, NULL                                    ) ;

    status = CPXsetintparam ( env, CPXPARAM_LPMethod, CPX_ALG_NET               ) ; // ??
    status = CPXlpopt       ( env, lp                                           ) ; // Aqui eh onde manda resolver
    status = CPXgetobjval   ( env, lp, &objval                                  ) ; // Coloca o valor da funcao objetivo na variavel objval
    status = CPXsolution    ( env, lp, &lpstat, &objval, x, NULL, NULL, NULL    ) ; // Coloca os valor das variaveis no vetor x

    printf ("Solution status %d\n", lpstat);
    printf ("Objective value %g\n", objval);
    printf ("Solution is:\n");
    for (j = 0; j < COLSORIG; j++) {
       printf ("x[%d] = %g\n", j + 1, x[j]);
    }

    status = CPXwriteprob (env, lp, "exemplo_supimpa.lp", NULL);  // Isso aqui salva um modelo do problema

    return 0;
}
