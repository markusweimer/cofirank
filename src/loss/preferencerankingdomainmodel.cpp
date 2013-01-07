#include "preferencerankingdomainmodel.hpp"
#include <cassert>

PreferenceRankingDomainModel::PreferenceRankingDomainModel(const ublas::matrix<Real>& X, const ublas::matrix<Real>& Y):X(X), Y(Y){}
PreferenceRankingDomainModel::~PreferenceRankingDomainModel(void){}

void PreferenceRankingDomainModel::ComputeLossGradient(cofi::WType& w, Real &loss, cofi::WType& grad){
    assert(w.size1() == grad.size1());
    assert(w.size2() == grad.size2()); // Gradient with respect to f
    ublas::matrix<Real> g(Y.size1(), 1);
    ComputeLossPartGradient(w, loss, g);
    
    // Make gradient with respect to w
    // grad = prod(trans(g), X);
    grad = prod(trans(X), g);
}


void PreferenceRankingDomainModel::ComputeLossPartGradient(cofi::WType& w, Real &loss, cofi::WType& grad){
    const ublas::matrix<Real> f = prod(X, w);
    grad.clear();
    loss = 0;
    for(size_t i=0; i<Y.size1(); i++){
        for (size_t j=0;j<Y.size1();j++){
            if (Y(i, 0) < Y(j, 0)){
                if (1 + f(i, 0)- f(j, 0) > 0){
                    loss += (Y(j, 0) - Y(i, 0))*(1 + f(i, 0) - f(j, 0));
                    grad(i, 0) += (Y(j, 0) - Y(i, 0));
                    grad(j, 0) -= (Y(j, 0) - Y(i, 0));
                }
            }
        }
    }
}






