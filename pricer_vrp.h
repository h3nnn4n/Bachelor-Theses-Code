#ifndef __SCIP_PRICER_VRP_H__
#define __SCIP_PRICER_VRP_H__

#include "objscip/objscip.h"
#include "scip/pub_var.h"

#include "types.h"

#include <vector>
#include <list>

using namespace std;
using namespace scip;

class ObjPricerVRP : public ObjPricer {
    public:

        ObjPricerVRP(
                SCIP*                               scip,
                const char*                         p_name,
                const int                           p_num_nodes,
                const int                           p_capacity,
                const vector< int >&                p_demand,
                const vector< vector<int> >&        p_distance,
                const vector< vector<SCIP_VAR*> >&  p_arc_var,
                const vector< vector<SCIP_CONS*> >& p_arc_con,
                const vector<SCIP_CONS* >&          p_part_con
                );

        ObjPricerVRP(
                SCIP* scip,
                const char* p_name,
                _csp *csp,
                _subproblem_info *subproblemInfo
                );

        virtual ~ObjPricerVRP();
        virtual SCIP_DECL_PRICERINIT(scip_init);
        virtual SCIP_DECL_PRICERREDCOST(scip_redcost);
        virtual SCIP_DECL_PRICERFARKAS(scip_farkas);

        SCIP_RETCODE pricing( SCIP*              scip, bool               isfarkas) const;
        SCIP_RETCODE add_tour_variable( SCIP*              scip, const list<int>&   tour) const;
        double find_shortest_tour( const vector< vector<double> >& length, list<int>&         tour) const;

    protected:
        inline int num_nodes() const {
            return _num_nodes;
        }

        inline int capacity() const {
            return _capacity;
        }

        inline int demand( const int i) const {
            return _demand[i];
        }

        inline double distance( const int i, const int j) const {
            return ( i > j ? _distance[i][j] : _distance[j][i] );
        }

        inline SCIP_VAR* arc_var( const int i, const int j) const {
            return ( i > j ? _arc_var[i][j] : _arc_var[j][i] );
        }

        inline SCIP_CONS* arc_con( const int i, const int j) const {
            return ( i > j ? _arc_con[i][j] : _arc_con[j][i] );
        }

        inline SCIP_CONS* part_con( const int i) const {
            return _part_con[i];
        }

        inline bool have_edge( const int i, const int j) const {
            return ( SCIPvarGetUbLocal( arc_var(i, j) ) > 0.5 );
        }

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
};

#endif
