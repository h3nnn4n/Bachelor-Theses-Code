/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                           */
/*                  This file is part of the program and library             */
/*         SCIP --- Solving Constraint Integer Programs                      */
/*                                                                           */
/*    Copyright (C) 2002-2016 Konrad-Zuse-Zentrum                            */
/*                            fuer Informationstechnik Berlin                */
/*                                                                           */
/*  SCIP is distributed under the terms of the ZIB Academic License.         */
/*                                                                           */
/*  You should have received a copy of the ZIB Academic License.             */
/*  along with SCIP; see the file COPYING. If not email to scip@zib.de.      */
/*                                                                           */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/**@file pricer_csp.cpp
 * @brief CSP pricer plugin
 * @author Andreas Bley
 * @author Marc Pfetsch
 */

#include "pricer_csp.h"

#include "subproblem.h"

#include <iostream>
#include <map>
#include <vector>

#include "scip/cons_linear.h"

using namespace std;
using namespace scip;

/** Constructs the pricer object with the data needed
 *
 *  An alternative is to have a problem data class which allows to access the data.
 */
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
        //{
        //for (int j = 0; j < i; ++j)
        //{
        //SCIP_CALL( SCIPgetTransformedVar(scip, _arc_var[i][j], &_arc_var[i][j]) ); [>lint !e732 !e747<]
        //SCIP_CALL( SCIPgetTransformedCons(scip, _arc_con[i][j], &_arc_con[i][j]) ); [>lint !e732 !e747<]
        //}
        //}
        //for (int i = 1; i < num_nodes(); ++i)
        //{
        //SCIP_CALL( SCIPgetTransformedCons(scip, _part_con[i], &_part_con[i]) ); [>lint !e732 !e747<]
        //}

        //SCIP_CALL( SCIPgetTransformedCons(scip, _part_con[i], &_part_con[i]) ); [>lint !e732 !e747<]
        SCIP_CALL( SCIPgetTransformedCons(scip, cons[i], &cons[i]) );
    }

    return SCIP_OKAY;
} /*lint !e715*/


/** perform pricing
 *
 *  @todo compute shortest length restricted tour w.r.t. duals
 */
SCIP_RETCODE ObjPricerCSP::pricing( SCIP* scip, bool isfarkas) const {
    for (int i = 0; i < csp->N; ++i) {
        subproblemInfo->duals[i] = SCIPgetDualsolLinear(scip, cons[i]);
    }

    subproblemInfo->mi = SCIPgetDualsolLinear(scip, cons[csp->N]);

    double reduced_cost;

    _journey journey = subproblem(csp, subproblemInfo, &reduced_cost);

//#ifdef SCIP_OUTPUT
    SCIP_CALL( SCIPwriteTransProblem(scip, "csp.lp", "lp", FALSE) );
//#endif

    if ( reduced_cost < 0.0 ) {
        subproblemInfo->usedJourneys[journey.covered] = true;
        return add_journey_variable(scip, journey);
    }

    return SCIP_OKAY;
}


/** Pricing of additional variables if LP is feasible.
 *
 *  - get the values of the dual variables you need
 *  - construct the reduced-cost arc lengths from these values
 *  return 0;
 *  - find the shortest admissible tour with respect to these lengths
 *  - if this tour has negative reduced cost, add it to the LP
 *
 *  possible return values for *result:
 *  - SCIP_SUCCESS    : at least one improving variable was found, or it is ensured that no such variable exists
 *  - SCIP_DIDNOTRUN  : the pricing process was aborted by the pricer, there is no guarantee that the current LP solution is optimal
 */
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


/** Pricing of additional variables if LP is infeasible.
 *
 *  - get the values of the dual Farks multipliers you need
 *  - construct the reduced-cost arc lengths from these values
 *  - find the shortest admissible tour with respect to these lengths
 *  - if this tour has negative reduced cost, add it to the LP
 */
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

    //[> add new variable to the list of variables to price into LP (score: leave 1 here) <]
    SCIP_CALL( SCIPaddPricedVar(scip, var, 1.0) );

    //[> add coefficient into the set partition constraints <]
    for (std::vector<int>::const_iterator it = journey.covered.begin(); it != journey.covered.end(); ++it) {
        assert( 0 <= *it && *it < csp->N );
        SCIP_CALL( SCIPaddCoefLinear(scip, cons[*it], var, 1.0) );
    }

    /* cleanup */
    SCIP_CALL( SCIPreleaseVar(scip, &var) );

    return SCIP_OKAY;
}

/** Computes a shortest admissible tour with respect to the given lengths. The function must return
 *  the computed tour via the parameter tour and the length (w.r.t. given lengths) of this tour as
 *  return parameter. The returned tour must be the ordered list of customer nodes contained in the
 *  tour (i.e., 2-5-7 for the tour 0-2-5-7-0).
 */
//namespace
//{

//[> types needed for prioity queue -------------------- <]
//static const SCIP_Real   eps = 1e-9;

//struct PQUEUE_KEY
//{
//int       demand;
//SCIP_Real length;

//PQUEUE_KEY() : demand(0), length(0.0) {}
//};

//bool operator< (const PQUEUE_KEY& l1, const PQUEUE_KEY& l2)
//{
//if ( l1.demand < l2.demand )
//return true;
//if ( l1.demand > l2.demand )
//return false;
//if ( l1.length < l2.length-eps )
//return true;
//[> not needed, since we return false anyway:
//if ( l1.length > l2.length+eps )
//return false;
//*/
//return false;
//}

//typedef int                                    PQUEUE_DATA; // node
//typedef pqueue<PQUEUE_KEY,PQUEUE_DATA>         PQUEUE;
//typedef PQUEUE::pqueue_item                    PQUEUE_ITEM;


//[> types needed for dyn. programming table <]
//struct NODE_TABLE_DATA
//{
//SCIP_Real             length;
//int                   predecessor;
//PQUEUE::pqueue_item   queue_item;

//NODE_TABLE_DATA( ) : length(0.0), predecessor(-1), queue_item( NULL ) {}
//};

//typedef int NODE_TABLE_KEY; // demand
//typedef std::map< NODE_TABLE_KEY, NODE_TABLE_DATA > NODE_TABLE;
//}


/** return negative reduced cost tour (uses restricted shortest path dynamic programming algorithm) 
 *
 *  The algorithm uses the priority queue implementation in pqueue.h. SCIP's implementation of
 *  priority queues cannot be used, since it currently does not support removal of elements that are
 *  not at the top.
 */
//SCIP_Real ObjPricerCSP::find_shortest_tour( const vector< vector<SCIP_Real> >& length, list<int>& tour) const {
    //return 0;
//}

//tour.clear();

//SCIPdebugMessage("Enter RSP - capacity: %d\n", capacity());

//[> begin algorithm <]
//PQUEUE               PQ;
//vector< NODE_TABLE > table(num_nodes()); [>lint !e732 !e747<]

//[> insert root node (start at node 0) <]
//PQUEUE_KEY       queue_key;
//PQUEUE_DATA      queue_data = 0;
//PQUEUE_ITEM      queue_item = PQ.insert(queue_key, queue_data);

//NODE_TABLE_KEY   table_key = 0;
//NODE_TABLE_DATA  table_entry;

//[> run Dijkstra-like updates <]
////while ( ! PQ.empty() )
////{
////[> get front queue entry <]
////queue_item = PQ.top();
////queue_key  = PQ.get_key (queue_item);
////queue_data = PQ.get_data(queue_item);
////PQ.pop();

////[> get corresponding node and node-table key <]
////const int       curr_node   = queue_data;
////const SCIP_Real curr_length = queue_key.length;
////const int       curr_demand = queue_key.demand;

////[> stop as soon as some negative length tour was found <]
////if ( curr_node == 0 && curr_length < -eps )
////break;

////[> stop as soon don't create multi-tours  <]
////if ( curr_node == 0 && curr_demand != 0 )
////continue;

////[> update all active neighbors <]
////for (int next_node = 0; next_node < num_nodes(); ++next_node)
////{
////if ( next_node == curr_node )
////continue;
////if ( have_edge( next_node, curr_node ) == false )
////continue;

////const int next_demand = curr_demand + demand(next_node);

////if ( next_demand > capacity() )
////continue;

////const SCIP_Real next_length = curr_length + ( curr_node > next_node ?      [>lint !e732 !e747<]
////length[curr_node][next_node] :  [>lint !e732 !e747<]
////length[next_node][curr_node] ); [>lint !e732 !e747<]

////NODE_TABLE& next_table = table[next_node]; [>lint !e732 !e747<]

////[> check if new table entry would be dominated <]
////bool skip = false;
////list<NODE_TABLE::iterator> dominated;

////for (NODE_TABLE::iterator it = next_table.begin(); it != next_table.end() && ! skip; ++it) [>lint !e1702<]
////{
////if ( next_demand >= it->first && next_length >= it->second.length - eps )
////skip = true;

////if ( next_demand <= it->first && next_length <= it->second.length + eps )
////dominated.push_front( it );
////}
////if ( skip )
////continue;

////[> remove dominated table and queue entries <]
////for (list<NODE_TABLE::iterator>::iterator it = dominated.begin(); it != dominated.end(); ++it) [>lint !e1702<]
////{
////PQ.remove( (*it)->second.queue_item );
////next_table.erase( *it );
////}

////[> insert new table and queue entry  <]
////queue_key.demand = next_demand;
////queue_key.length = next_length;
////queue_data       = next_node;

////queue_item = PQ.insert(queue_key, queue_data);

////table_key               = next_demand;
////table_entry.length      = next_length;
////table_entry.predecessor = curr_node;
////table_entry.queue_item  = queue_item;

////next_table[table_key] = table_entry;

////#ifdef SCIP_OUTPUT
////printf("new entry  node = %d  demand = %d  length = %g  pref = %d\n", next_node, next_demand, next_length, curr_node);
////#endif
////}
////}

//SCIPdebugMessage("Done RSP DP.\n");

//table_entry.predecessor = -1;
//table_entry.length      = 0;
//int curr_node = 0;

//[> find most negative tour <]
//for (NODE_TABLE::iterator it = table[0].begin(); it != table[0].end(); ++it) [>lint !e1702 !e732 !e747<]
//{
//if ( it->second.length < table_entry.length )
//{
//table_key   = it->first;
//table_entry = it->second;
//}
//}
//SCIP_Real tour_length = table_entry.length;

//while ( table_entry.predecessor > 0 )
//{
//table_key -= demand(curr_node);
//curr_node  = table_entry.predecessor;
//tour.push_front(curr_node);
//table_entry = table[curr_node][table_key]; [>lint !e732 !e747<]
//}

//SCIPdebugMessage("Leave RSP  tour length = %g\n", tour_length);

//return tour_length;
//}
