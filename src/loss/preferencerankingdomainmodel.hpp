#ifndef _PREFERENCERANKINGDOMAINMODEL_HPP_
#define _PREFERENCERANKINGDOMAINMODEL_HPP_

#include "cofilossfunction.hpp"

/**
 * PreferenceRanking Loss.
 *
 */

class PreferenceRankingDomainModel : public CofiLossFunction{
    
public:
    /**
     * @param X the samples to learn from
     * @param Y the labels for the given samples
     */
    
    PreferenceRankingDomainModel(const ublas::matrix<Real>& X, const ublas::matrix<Real>& Y);
    ~PreferenceRankingDomainModel(void);

    void ComputeLossGradient(cofi::WType& w, Real &loss, cofi::WType& grad);
    void ComputeLossPartGradient(cofi::WType& w, Real &loss, cofi::WType& grad);
    
    
    
private:
    // Attributes
    const ublas::matrix<Real>& X;
    const ublas::matrix<Real>& Y;
};

#endif
