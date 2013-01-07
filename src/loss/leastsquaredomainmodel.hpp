#ifndef _LEASTSQUAREDOMAINMODEL_HPP_
#define _LEASTSQUAREDOMAINMODEL_HPP_

#include "cofilossfunction.hpp"

/**
 * LeastSquare Loss.
 *
 */

class LeastSquareDomainModel : public CofiLossFunction{
    
public:
    /**
     * @param X the samples to learn from
     * @param Y the labels for the given samples
     */
    
    LeastSquareDomainModel(const ublas::matrix<Real>& X, const ublas::matrix<Real>& Y);
    ~LeastSquareDomainModel(void);
    
    
    void ComputeLossGradient(cofi::WType& w, Real &loss, cofi::WType& grad);
    void ComputeLossPartGradient(cofi::WType& w, Real &loss, cofi::WType& grad);
    
    
private:
    // Attributes
    const ublas::matrix<Real>& X;
    const ublas::matrix<Real>& Y;
};

#endif
