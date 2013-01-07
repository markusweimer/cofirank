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
 * Created      : 21/05/2007
 *
 * Last Updated :
 */
#ifndef _USERTRAINER_HPP_
#define _USERTRAINER_HPP_

#include "core/types.hpp"
#include "cofi/problem.hpp"


namespace cofi{
    /**
     * Subspace decent in U.
     *
     */
    class UserTrainer {
        
    public:
        /**
         * Runs the taining procedure for all users.
         * @param p The Problem to work on
         * @param t The current iteration we are in
         * @param lambda The regularization parameter
         */
        Real run(cofi::Problem& p, size_t t, Real lambda);

    };
}
#endif
