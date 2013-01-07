/* The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Authors      : Markus Weimer       (cofirank@weimo.de)
 *
 * Created      : 27/05/2007
 *
 * Last Updated :
 */

#ifndef _USERITERATOR_HPP_
#define _USERITERATOR_HPP_

#include "core/types.hpp"
#include "cofi/problem.hpp"
#include "loss/cofilossfunction.hpp"


namespace cofi{
    
    // Forward declaration.
    class Problem;
    /**
     * An iterator over the BMRM problems posed by the users.
     *
     *
     * For ech user i:
     *  extract the movies of M which that user has actually rated
     *  extract U[i] as w
     *  build Y such that Y[i] is the rating of the movie whose features are in X[i]
     */
    class UserIterator {
    public:
        
        enum Phase{TRAINING, TESTING};
        /**
         * Construct a new iterator
         *
         */
        UserIterator(cofi::Problem& p, Phase phase);
        
        /**
         * Deletes all temporary matrices created.
         */
        ~UserIterator();
        
        /**
         * @return true, if there is another user to iterate to.
         */
        bool hasNext(void);
        
        /**
         * Advances to the next user.
         */
        void advance(void);
        
        /**
         * Gets the matrix X for the current user
         *
         * @return the matrix X for the current user
         */
        inline ublas::matrix<Real>& getX(void) {return *X;}
        
        /**
         * @return the matrix Y for the current user
         */
        inline ublas::matrix<Real>& getY(void) {return *Y;}
        
        /**
         * @return the parameter vector for the current user.
         */
        cofi::WType& getW(void);

        
        /**
         * @return the loss function for the current user
         */
        CofiLossFunction& getLoss(void);
        
        /**
         * @return the loss function which incorporates the weights as reported by the Problem
         */
        CofiLossFunction& getWeightedLoss(void);

        /**
         * Computes the current prediction into the given matrix F
         */
        void predict(ublas::matrix<Real>& F);
        
        /**
         * @return the current row in U the iterator is pointing to
         */
        const size_t getRowInU(void) {
            return nextRow-1;
        }
        
        void updateW(void);
        
        
        
        
    private:
        /**
         * Deletes all interemediate structures. Namely:
         * X, Y, W, loss
         */
        void clear(void);
        Problem &p;
        const Phase phase;
        
        typedef cofi::DType::iterator1 rowIteratorType;
        typedef cofi::DType::iterator2 colIteratorType;
        rowIteratorType dRows;
        size_t nextRow;
        ublas::matrix<Real>* X;
        ublas::matrix<Real>* Y;
        cofi::WType* W;
        
        CofiLossFunction* loss;
        CofiLossFunction* weightedLoss; // The loss which includes the weight
        cofi::UType SA; // S times A
        
        
        
    };
}
#endif /* _USERITERATOR_HPP_ */
