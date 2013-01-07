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
 * Created      : 20/09/2007
 *
 * Last Updated :
 */
#ifndef _COFIEVALUATOR_HPP_
#define _COFIEVALUATOR_HPP_

#include <string>
#include <vector>
#include <map>

#include "core/types.hpp"
#include "cofi/problem.hpp"


/**
 * Evaluation routines for CofiBMRM.
 */
namespace cofi{
    class CofiEvaluator {
    public:

        virtual ~CofiEvaluator(void) {};
        
        /**
         * @return the name of the evaluation measure
         */
        virtual std::vector<std::string> names(void) = 0;
        
        /**
         * Evaluates based on current state of problem and writes to the stream.
         * @param iter the iterator over the data to evaluate on.
         * @return the evaluation result.
         */
        virtual void eval(cofi::UserIterator& iter, std::map<std::string, double>& results)=0;

        
    };
}
#endif
