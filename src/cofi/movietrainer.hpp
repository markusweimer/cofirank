/* Copyright (c) 2006, National ICT Australia
 * All rights reserved.
 *
 * The contents of this file are subject to the Mozilla Public License Version
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
 * Created      :
 *
 * Last Updated : 21/06/2007
 */

#ifndef _MOVIETRAINER_HPP_
#define _MOVIETRAINER_HPP_

#include "core/types.hpp"
#include "cofi/problem.hpp"


namespace cofi{
    /**
     * Subspace decent in M.
     */
    class MovieTrainer{
        
    public:
        /**
         * Runs the taining procedure for the movies.
         *
         * This method will alter the movie Matrix M
         * @param p The Problem to work on
         */
        Real run(cofi::Problem& p, size_t t, Real lambda);
        
    };
}

#endif
