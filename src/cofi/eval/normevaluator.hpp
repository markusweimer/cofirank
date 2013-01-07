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
#ifndef _NORMEVALUATOR_H
#define	_NORMEVALUATOR_H

#include <string>
#include <vector>
#include <map>

#include "core/types.hpp"

#include "cofi/problem.hpp"
#include "dataindependentevaluator.hpp"
namespace cofi{
    /**
     * Evaluates the norm of M and U.
     *
     */
    class NormEvaluator: public DataIndependentEvaluator {
    public:
        
        NormEvaluator(void);
        virtual ~NormEvaluator(void);
        
        /**
         * @return the name of the evaluation measure
         */
        virtual std::vector<std::string> names(void);
        
        /**
         * Evaluates based on current state of problem and writes to the stream.
         * @param p the problem which holds the data to evaluate on.
         * @return the evaluation result.
         */
        void eval(cofi::Problem& p, std::map<std::string, double>& results);
        
    };
}
#endif	/* _NORMEVALUATOR_H */
