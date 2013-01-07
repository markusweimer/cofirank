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
#ifndef __NDCGEVALUATOR_HPP__
#define __NDCGEVALUATOR_HPP__
#include "cofievaluator.hpp"

#include <string>
#include <vector>
#include <map>

namespace cofi{
    /**
     * A evaluator which determines the average NDCG for all the rows in U
     *
     * @author Markus Weimer
     */
    class NDCGEvaluator : public CofiEvaluator {
    public:
        
        NDCGEvaluator(void);
        
        /**
         * @return "ndcg"
         */
        std::vector<std::string> names(void);
        
        void eval(cofi::UserIterator& iter, std::map<std::string, double>& results);
        
    private:
        size_t truncation;
        std::string name;
        bool bigKWarned; // True if we warned the user that k is bigger than Y for some users.
    };
    
}
#endif
