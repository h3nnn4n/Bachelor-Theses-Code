#ifndef __SCIP_PRICER_CSP_H__
#define __SCIP_PRICER_CSP_H__

#include "objscip/objscip.h"
#include "scip/pub_var.h"

#include "types.h"

#include <vector>
#include <list>

using namespace std;
using namespace scip;

class ObjPricerCSP : public ObjPricer {
    public:

        ObjPricerCSP(
                SCIP* scip,
                const char* p_name,
                _csp* _csp,
                _subproblem_info* _subproblemInfo,
                SCIP_CONS** _cons
                );

        virtual ~ObjPricerCSP();
        virtual SCIP_DECL_PRICERINIT(scip_init);
        virtual SCIP_DECL_PRICERREDCOST(scip_redcost);
        virtual SCIP_DECL_PRICERFARKAS(scip_farkas);

        SCIP_RETCODE pricing( SCIP* scip, bool isfarkas) const;
        SCIP_RETCODE add_journey_variable( SCIP* scip, const _journey journey) const;

    private:
        int                    _num_nodes;
        int                    _capacity;
        vector< int >          _demand;
        vector< vector<int> >  _distance;

        vector< vector<SCIP_VAR*> >  _arc_var;
        vector< vector<SCIP_CONS*> > _arc_con;
        vector< SCIP_CONS* >         _part_con;

        _csp *csp;
        _subproblem_info *subproblemInfo;

        SCIP_CONS** cons;
};

#endif
