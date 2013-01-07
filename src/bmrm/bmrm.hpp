#ifndef _BMRM_HPP_
#define _BMRM_HPP_

#include <string>
#include "core/types.hpp"
#include "solver/innersolver.hpp"
#include "solver/daifletcherpgm.hpp"
#include "lossfunction.hpp"

/**   Class for BMRM solver.
 *    This type of solver iteratively builds up a convex lower-bound of the 
 *      objective function, and performs minimization on the lower-bound.
 */
class BMRM {
public:

    // Constructors
    BMRM(LossFunction& lossFunction, const Real lambda, const size_t dimW);

    // Destructor
    virtual ~BMRM();

    // Methods
    Real train(ublas::matrix<Real>& w);

    /**
     * Set convergence cirtaria
     *
     * @param gammaTol TODO
     * @param epsilonTol TODO
     * @maram maxIter Maximum number of iterations of the algorithm
     */
    void setConvergence(double gammaTol = 0.01, double epsilonTol = 1e-2, double relEpsilonTol=0.1, double relGammaTol=0.1, int maxIter = 4000);



protected:

    /** Maximum number of BMRM iteration
     */
    unsigned int maxNumOfIter;

    /** Tolerance for epsilon termination criterion.
     *  epsilon = min_{t' <= t} J(w_{t'}) - J_t(w_t),
     *  where J_t is piece-wise linear approx. of J
     */
    double epsilonTol;

    /** Relative epsilonTol.
     *  Relative epsilon = epsilon / min_{t' <= t} J(w_{t'})
     */
    double relEpsilonTol;

    /**  Tolerance for gamma termination criterion.
     *   gamma = J(w_t) - J_t(w_t)
     */
    double gammaTol;

    LossFunction& lossFunction; // LOSS

    /** Relative gammaTol.
     *  relGamma = gamma / J(w_t)
     */
    double relGammaTol;

    /** Regularization constant
     */
    double lambda;


    size_t dimW;

    /** Types of checkpoint mode.
     *  KEEP_ALL -- Keep a checkpoint model after every $checkpointInterval$ iterations
     *  KEEP_LATEST -- Keep only the latest model
     */
    enum CHECKPOINT_MODE {
        KEEP_ALL, KEEP_LATEST
    };

    /* BMRM inner solver
     */
    InnerSolver* innerSolver; // pointer to inner solver object


};

#endif
