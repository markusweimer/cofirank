//
// File:   adaptiveregularizationlosswrapper.h
// Author: weimer
//
// Created on January 23, 2008, 7:23 PM
//

#ifndef _ADAPTIVEREGULARIZATIONLOSSWRAPPER_H
#define	_ADAPTIVEREGULARIZATIONLOSSWRAPPER_H

#include "cofilossfunction.hpp"

// TODO: Document this file
namespace cofi{
    /**
     */
    class AdaptiveRegularizationLossWrapper: public CofiLossFunction{
    public:
        
        AdaptiveRegularizationLossWrapper(const Real weight, CofiLossFunction& loss):weight(weight), lossFunction(loss){};
        
        ~AdaptiveRegularizationLossWrapper(void){};
        
        
        /**
         * Compute the loss and the gradient with respect to w for the current solution w
         *
         * @param w (in) the current solution.
         * @param loss (out) the computed loss value.
         * @param grad (out) the computed gradient.
         *
         */
        void ComputeLossGradient(WType& w, Real &loss, ublas::matrix<Real>& grad);
        
        /**
         * This method is only here to make this class compatible with the
         * DomainModel interface.
         *
         * It should never be called and raises an exception if so.
         */
        void ComputeLossPartGradient(WType& w, Real &loss, ublas::matrix<Real>& grad);
        
    private:
        const Real weight;
        CofiLossFunction& lossFunction;
    };
    
}
#endif	/* _ADAPTIVEREGULARIZATIONLOSSWRAPPER_H */

