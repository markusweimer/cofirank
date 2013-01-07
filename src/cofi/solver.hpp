//
// File:   solver.h
// Author: weimer
//
// Created on February 1, 2008, 5:39 PM
//

#ifndef _SOLVER_H
#define	_SOLVER_H
#include <loss/cofilossfunction.hpp>
#include "utils/configuration.hpp"
#include <boost/numeric/ublas/matrix.hpp>

namespace ublas = boost::numeric::ublas;

namespace cofi{
    
    /**
     * Central HUB for the solvers.
     *
     * This class dispatches calls to solvers to their actual implementation.
     * CofiRank can use different solvers for the underlying convex optimization 
     * problems. This is the place where the selection of solvers is made based 
     * on the configuration file.
     *
     * @author Markus Weimer <cofirank@weimo.de>
     *
     */
    class Solver{
    public:
        enum Solvers{bmrm, sgd, smd};
        Solver(void);
        ~Solver(void);
        
        /**
         * @param w the parameters to optimize.
         * @param loss the lossfunction to use.
         * @param lambda the regularizer factor
         */
        Real optimize(cofi::WType& w, LossFunction& loss, const Real lambda, const size_t t, ublas::matrix<Real>* X = NULL, ublas::matrix<Real>* Y=NULL);
    private:
        Solvers choosenSolver;
    };
}

#endif	/* _SOLVER_H */

