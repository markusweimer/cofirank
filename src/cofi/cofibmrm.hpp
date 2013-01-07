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
 * Created      : 28/05/2007
 *
 * Last Updated :
 */
#ifndef _COFIBMRM_HPP_
#define _COFIBMRM_HPP_

#include <vector>
#include <string>


#include "core/types.hpp"
#include "cofi/movietrainer.hpp"
#include "cofi/usertrainer.hpp"
#include "cofi/problem.hpp"

namespace cofi{
    
    /**
     * The main class of cofirank
     * 
     * This class represents the actual algorithm. To do so, it uses two other 
     * classes, UserTrainer and MovieTrainer. Those implement the subspace 
     * decent in U and M respectively.
     *
     */
    class COFIBMRM {
    public:
        /**
         * param p the problem to work on.
         */
        COFIBMRM(cofi::Problem& p);
        
        /**
         * Trains using the CoFiBMRM algorithm
         */
        void train();
        

    private:
        /**
         * @return true, iff the algorithm converged. false otherwise
         */
        bool isConverged();
        
        /**
         * The loop counter
         */
        size_t iteration;

        /**
         * Access to the real data
         */
        cofi::Problem& p;
        
        /**
         * The folder in which all the results are stored
         */
        std::string outFolder;
        
        /**
         * User and movie lambdas
         */
        Real movieLambda;
        Real userLambda;
        
        /**
         * The parameter of the convergence criterion
         */
        size_t minIterations;
        size_t maxIterations;
        Real allowedDivergence;

        
        /**
         * Data structures for the convergence criterion
         */
        std::vector<Real> objectiveFunctionValues; // The objective function value
        std::vector<Real> userLambdas;             // The lambda used in the user phase per iteration
        std::vector<Real> movieLambdas;            // The lambda used in the user phase per iteration
        std::vector<Real> movieLosses;             // The loss of the movie phase per iteration
        std::vector<Real> userLosses;              // The loss of the user phase per itertion
        std::vector<Real> movieNorms;              // The norm of M per iteration
        std::vector<Real> userNorms;               // The norm of U per iteration
    };
}
#endif /* _COFIBMRM_HPP_ */
