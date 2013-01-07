#ifndef _DomainModel_HPP_
#define _DomainModel_HPP_

#include "core/types.hpp"
#include "bmrm/lossfunction.hpp"

/**
 * Base class for domain models
 */

class CofiLossFunction : public LossFunction{
    
public:
    
    /// Destructor
    virtual ~CofiLossFunction() {}
    
    
    /**
     * Compute the loss and the partial gradient with respect to F for the current solution w
     *
     * @param w (in) the current solution.
     * @param loss (out) the computed loss value.
     * @param grad (out) the computed partial gradient.
     *
     */
    virtual void ComputeLossPartGradient(cofi::WType& w, Real& loss, ublas::matrix<Real>& grad) = 0;
};

#endif
