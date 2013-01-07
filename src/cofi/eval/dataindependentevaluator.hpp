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
 * Created      : 16/01/2008
 *
 * Last Updated :
 */
#ifndef _METAEVALUATOR_H
#define	_METAEVALUATOR_H
#include <vector>
#include <string>

#include "cofi/problem.hpp"

namespace cofi{
    /**
     * Base class for evaluators which is not dependent on the actual test data.
     */
    class DataIndependentEvaluator{
    public:
        virtual ~DataIndependentEvaluator(void) {};
        
        /**
         * @return the name of the evaluation measure
         */
        virtual std::vector<std::string> names(void) = 0;
        
        /**
         * Evaluates based on current state of problem and writes to the stream.
         * @param p the problem which holds the data to evaluate on.
         * @return the evaluation result.
         */
        virtual void eval(cofi::Problem& p, std::map<std::string, double>& results) = 0;
    };
}


#endif	/* _METAEVALUATOR_H */

