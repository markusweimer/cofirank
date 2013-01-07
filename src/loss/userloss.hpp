/* 
 * File:   userloss.hpp
 * Author: weimer
 *
 * Created on May 6, 2008, 5:47 PM
 */

#ifndef _USERLOSS_HPP
#define	_USERLOSS_HPP

#include <bmrm/lossfunction.hpp>
#include "cofilossfunction.hpp"
namespace cofi {

    /**
     * A wrapper around the actual loss to be used in the user phase.
     *
     * For now, it only masks the movie offset from the optimizer
     */
    class UserLoss : public LossFunction {
    public:
        
        /**
         * @param realLoss the loss to mask.
         * @param useOffset whether or not an offset shall be used.
         */
        UserLoss(CofiLossFunction& realLoss, bool useOffset);
        
        /**
         * Computes the loss and gradient using the real loss function.
         *
         * If the offset is used, it does its thing to deal with it.
         */
        void ComputeLossGradient(cofi::WType& w, Real& loss, cofi::WType& grad);
    
    private:
        
        /**
         * The real loss function to be used for the computations.
         */
        CofiLossFunction& realLoss;
        
        /**
         * Whether or not the offset is to be used.
         */
        const bool useOffset;
    };
}


#endif	/* _USERLOSS_HPP */

