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
 * Created      : 12/12/2007
 *
 * Last Updated :
 */
#ifndef _BINARYEVALUATOR_HPP_
#define _BINARYEVALUATOR_HPP_

#include <string>
#include <vector>
#include <map>

#include "core/types.hpp"

#include "cofi/problem.hpp"
#include "cofievaluator.hpp"
#include "cofi/useriterator.hpp"
namespace cofi{
    /**
     * Evaluation for binary classification.
     *
     * Determines the folllowing values:
     *
     *  - False Positives
     *  - False Negatives
     *  - Correct Classifications
     *  - All Classifications
     *
     * It is assumed that the classes are (-\inf ... 0) and [0 ... \inf)
     *
     */
    class BinaryEvaluator: public CofiEvaluator {
    public:
        
        BinaryEvaluator(void);
        virtual ~BinaryEvaluator(void);
        
        /**
         * @return the name of the evaluation measure
         */
        std::vector<std::string> names(void);

        void eval(cofi::UserIterator& iter, std::map<std::string, double>& results);
    };
}
#endif
