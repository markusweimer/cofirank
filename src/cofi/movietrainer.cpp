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
 *                Markus Weimer (cofirank@weimo.de)
 *
 * Created      :
 *
 * Last Updated : 21/06/2007
 */

#include <cassert>
#include "movietrainer.hpp"
#include "solver.hpp"
#include "loss/moviephaselossfunction.hpp"

Real cofi::MovieTrainer::run(cofi::Problem& p, size_t t, Real lambda){
    assert(lambda>0);
    assert(t>=0);
    // Offset management:
    // If we have an offset, we need to make sure that bmrm does not optimize it
    // away. We do two things to assure this:
    //
    // (1) We set the column of the bias to zero before calling BMRM and set it
    //     to 1 after optimization.
    // (2) In the loss computation, we set it to 1 for the loss and gradient
    //     computation.
    
    // Set the column of the bias to 0 such that it is ignored by bmrm
//    if(p.usingOffset()){
//        p.setMLastColumnZero();
//    }
    
    MoviePhaseLossFunction m(p);
    cofi::Solver solver;

    const Real loss = solver.optimize(p.getM(), m, lambda, t);

    if(p.usingUserOffset()){ // Set the column of the bias to 1 again
        p.setUserOffsetColumnInMToOne();
    }
    
    return loss;
    
}
