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
 * Created      : 05/01/2008
 *
 * Last Updated :
 */
#ifndef _MEANSQUAREDERROR_H
#define	_MEANSQUAREDERROR_H

#include <string>
#include <vector>
#include <map>

#include "core/types.hpp"

#include "cofi/problem.hpp"
#include "cofievaluator.hpp"

namespace cofi{
    /**
     * Evaluates U and M based on the Rooted Mean Squared Error (RMSE)
     */
    class MSEEvaluator : public CofiEvaluator{
    public:
        MSEEvaluator(void){};
        virtual ~MSEEvaluator(void){};
        
        /**
         * @return the name of the evaluation measure
         */
        std::vector<std::string> names(void);

        void eval(cofi::UserIterator& iter, std::map<std::string, double>& results);
    };
    
}
#endif	/* _MEANSQUAREDERROR_H */

