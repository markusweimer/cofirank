//
// File:   loss.h
// Author: weimer
//
// Created on February 1, 2008, 3:09 PM
//

#ifndef _LOSS_H
#define	_LOSS_H
#include "core/types.hpp"

class LossFunction{
public:
    
    virtual ~LossFunction(void){};
    
    /**
     * Compute the loss and the gradient for the current solution w
     *
     * @param w (in) the current solution.
     * @param loss (out) the computed loss value.
     * @param grad (out) the computed gradient.
     *
     */
    virtual void ComputeLossGradient(cofi::WType& w, Real &loss, cofi::WType& grad)=0;
    
    /**
     * Computes the prediction of this loss with the given W and X into F.
     *
     * The default implementation in this class just calls F=prod(W,X)
     *
     * @param W the parameters for the model
     * @param X the data
     * @param F the computed prediction.
     *
     */
    virtual inline void predict(cofi::WType& W, cofi::WType& X, ublas::matrix<Real>& F){
        F = prod(X,W);
    }
    
};


#endif	/* _LOSS_H */

