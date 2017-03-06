#include <assert.h>

#include "pricer_csp.h"

#include "types.h"
#include "utils.h"
/*#include "greedy_heur.h"*/
/*#include "tabu_search.h"*/
/*#include "exact_subproblem.h"*/
/*#include "simmulated_annealing.h"*/
/*#include "ant_colony_optimization.h"*/


#define PRICER_NAME            "CSP_pricer"
#define PRICER_DESC            "Pricer for the CSP using (meta)heuristics and a exact solution as fallback"
#define PRICER_PRIORITY        0
#define PRICER_DELAY           TRUE     /* only call pricer if all problem variables have non-negative reduced costs */

struct SCIP_PricerData {
    _subproblem_info *sp;
    _csp *csp;
};

static SCIP_DECL_PRICERREDCOST(pricerRedcostXyz) {
   SCIPerrorMessage("method of xyz variable pricer not implemented yet\n");
   SCIPABORT(); /*lint --e{527}*/

   return SCIP_OKAY;
}

//static SCIP_DECL_PRICERINITSOL(pricerInitsolXyz) {  [>lint --e{715}<]
   //SCIPerrorMessage("method of xyz variable pricer not implemented yet\n");
   //SCIPABORT(); [>lint --e{527}<]

   //return SCIP_OKAY;
//}

#define pricerCopyXyz NULL
#define pricerFreeXyz NULL
#define pricerInitXyz NULL
#define pricerExitXyz NULL
#define pricerInitsolXyz NULL
#define pricerExitsolXyz NULL
#define pricerFarkasXyz NULL

SCIP_RETCODE SCIPincludePricerXyz( SCIP* scip ) {
   SCIP_PRICERDATA* pricerdata;
   SCIP_PRICER* pricer;

   pricerdata = NULL;

   pricer = NULL;

   SCIP_CALL( SCIPincludePricerBasic(scip, &pricer, PRICER_NAME, PRICER_DESC, PRICER_PRIORITY, PRICER_DELAY,
         pricerRedcostXyz, pricerFarkasXyz, pricerdata) );
   assert(pricer != NULL);

   SCIP_CALL( SCIPsetPricerCopy(scip, pricer, pricerCopyXyz) );
   SCIP_CALL( SCIPsetPricerFree(scip, pricer, pricerFreeXyz) );
   SCIP_CALL( SCIPsetPricerInit(scip, pricer, pricerInitXyz) );
   SCIP_CALL( SCIPsetPricerExit(scip, pricer, pricerExitXyz) );
   SCIP_CALL( SCIPsetPricerInitsol(scip, pricer, pricerInitsolXyz) );
   SCIP_CALL( SCIPsetPricerExitsol(scip, pricer, pricerExitsolXyz) );

   SCIP_CALL( SCIPactivatePricer(scip, SCIPfindPricer(scip, PRICER_NAME)) );

   return SCIP_OKAY;
}
