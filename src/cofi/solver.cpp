#include "solver.hpp"
#include <bmrm/bmrm.hpp>
#include <core/cofiexception.hpp>


cofi::Solver::Solver(void) {
}


Real cofi::Solver::optimize(cofi::WType& w, LossFunction& loss, const Real lambda, const size_t t, ublas::matrix<Real>* X, ublas::matrix<Real>* Y) {
    // should we but this in the constructor ?
    // configuring the solver should be done once per phase i.e. user/movie
    Configuration& conf = Configuration::getInstance();


    const double _gammaTol = conf.getDouble("bmrm.minProgress");
    const double _epsilonTol = conf.getDouble("bmrm.minOptimProgress");
    const int _maxIter = conf.getInt("bmrm.maxNumberOfIterations");
    const double _relGammaTol = conf.getDouble("bmrm.minRelativeProgress");
    const double _relEpsilonTol = conf.getDouble("bmrm.minRelativeOptimProgress");

    size_t dimW2 = 0;
    // dimW2 should reflect the dimension of w
    if (w.size2() == 0) {
        dimW2 = w.size1();
    } else {
        dimW2 = w.size1() * w.size2();
    }

    BMRM b(loss, lambda, dimW2);
    b.setConvergence(_gammaTol, _epsilonTol, _relEpsilonTol, _relGammaTol, _maxIter);

    return b.train(w);

}


cofi::Solver::~Solver(void) {
}
