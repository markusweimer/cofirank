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
 * Authors      : Alexandros Karatzoglou (alexis@ci.tuwien.ac.at)
 *                Markus Weimer
 *
 * Created      : 21/06/2007
 *
 * Last Updated :
 */

#ifndef _DOMAINWRAPPER_HPP_
#define _DOMAINWRAPPER_HPP_

#include "bmrm/lossfunction.hpp"

#include "cofi/problem.hpp"


namespace cofi{
    /**
     * DomainWrapper.
     *
     */
    class MoviePhaseLossFunction : public LossFunction {
        
    public:
        MoviePhaseLossFunction(cofi::Problem& p);
        
        ~MoviePhaseLossFunction() {}
        void ComputeLossGradient(cofi::WType& w, Real &loss, cofi::WType& grad);
        

        
        
    private:
        // Attributes
        cofi::Problem& p;
        typedef cofi::DType::const_iterator1 itr1;
        typedef cofi::DType::const_iterator2 itr2;
        unsigned int nUser;
        unsigned int nMovies;
        
    };
}
#endif
