#include "pricer_csp.h"

#include "subproblem.h"

#include <iostream>
#include <map>
#include <vector>

#include "scip/cons_linear.h"

using namespace std;
using namespace scip;

ObjPricerCSP::ObjPricerCSP(
        SCIP* scip,
        const char* p_name,
        _csp* _csp,
        _subproblem_info* _subproblemInfo,
        SCIP_CONS** _cons
        ):
    ObjPricer(scip, p_name, "Finds tour with negative reduced cost.", 0, TRUE),
    csp(_csp),
    subproblemInfo(_subproblemInfo),
    cons(_cons)
{}

/** Destructs the pricer object. */
ObjPricerCSP::~ObjPricerCSP()
{}


/** initialization method of variable pricer (called after problem was transformed)
 *
 *  Because SCIP transformes the original problem in preprocessing, we need to get the references to
 *  the variables and constraints in the transformed problem from the references in the original
 *  problem.
 */
SCIP_DECL_PRICERINIT(ObjPricerCSP::scip_init) {
    for (int i = 0; i < csp->N; ++i){
        SCIP_CALL( SCIPgetTransformedCons(scip, cons[i], &cons[i]) );
    }

    return SCIP_OKAY;
}


// The pricing function
SCIP_RETCODE ObjPricerCSP::pricing( SCIP* scip, bool isfarkas) const {
    for (int i = 0; i < csp->N; ++i) {
        subproblemInfo->duals[i] = SCIPgetDualsolLinear(scip, cons[i]);
    }

    subproblemInfo->mi = SCIPgetDualsolLinear(scip, cons[csp->N]);

    double reduced_cost;

    _journey journey = subproblem(csp, subproblemInfo, &reduced_cost);

    //SCIP_CALL( SCIPwriteTransProblem(scip, "csp.lp", "lp", FALSE) );

    if ( reduced_cost < 0.0 ) {
        subproblemInfo->usedJourneys[journey.covered] = true;
        return add_journey_variable(scip, journey);
    }

    return SCIP_OKAY;
}


// Pricing of additional variables if LP is feasible.
SCIP_DECL_PRICERREDCOST(ObjPricerCSP::scip_redcost) {
    //SCIPdebugMessage("call scip_redcost ...\n");
    SCIPinfoMessage(scip, NULL, "call scip_redcost ...\n");

    /* set result pointer, see above */
    *result = SCIP_SUCCESS;

    /* call pricing routine */
    SCIP_CALL( pricing(scip, false) );

    //for (int i = 1; i < csp->N; ++i) {
        //SCIPinfoMessage(scip, NULL, "dual_%3d: %6.2f\n", i, SCIPgetDualsolLinear(scip, cons[i]));
    //}
    //SCIPinfoMessage(scip, NULL, "mi_  %3d: %6.2f\n", csp->N, SCIPgetDualsolLinear(scip, cons[csp->N]));
    //puts("exiting scip_redcost\n");

    return SCIP_OKAY;
} /*lint !e715*/


// Pricing of additional variables if LP is infeasible.
// This should never be called
SCIP_DECL_PRICERFARKAS(ObjPricerCSP::scip_farkas) {
    //SCIPdebugMessage("call scip_farkas ...\n");
    SCIPinfoMessage(scip, NULL, "call scip_farkas ...\n");

    /* call pricing routine */
    SCIP_CALL( pricing(scip, true) );

    return SCIP_OKAY;
} /*lint !e715*/

/** add tour variable to problem */
SCIP_RETCODE ObjPricerCSP::add_journey_variable( SCIP* scip, const _journey journey) const {
    /* create meaningful variable name */
    char tmp_name[255];
    char var_name[255];
    (void) SCIPsnprintf(var_name, 255, "journey_");
    for (std::vector<int>::const_iterator it = journey.covered.begin(); it != journey.covered.end(); ++it) {
        strncpy(tmp_name, var_name, 255);
        (void) SCIPsnprintf(var_name, 255, "%s_%d", tmp_name, *it);
    }
    SCIPinfoMessage(scip, NULL, "new variable <%s>\n", var_name);

    /* create the new variable: Use upper bound of infinity such that we do not have to care about
     * the reduced costs of the variable in the pricing. The upper bound of 1 is implicitly satisfied
     * due to the set partitioning constraints.
     */
    SCIP_VAR* var;
    SCIP_CALL( SCIPcreateVar(scip, &var, var_name,
                0.0,                     // lower bound
                SCIPinfinity(scip),      // upper bound
                0.0,                     // objective
                SCIP_VARTYPE_CONTINUOUS, // variable type
                false, false, NULL, NULL, NULL, NULL, NULL) );

    /* add new variable to the list of variables to price into LP (score: leave 1 here) */
    SCIP_CALL( SCIPaddPricedVar(scip, var, 1.0) );

    /* add coefficient into the set partition constraints */
    for (std::vector<int>::const_iterator it = journey.covered.begin(); it != journey.covered.end(); ++it) {
        assert( 0 <= *it && *it < csp->N );
        SCIP_CALL( SCIPaddCoefLinear(scip, cons[*it], var, 1.0) );
    }

    /* cleanup */
    SCIP_CALL( SCIPreleaseVar(scip, &var) );

    return SCIP_OKAY;
}
