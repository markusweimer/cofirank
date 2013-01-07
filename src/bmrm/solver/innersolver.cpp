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
 * Authors      : Choon Hui Teo (ChoonHui.Teo@anu.edu.au)
 *                Alexandos Karatzoglou (alexis@ci.tuwien.ac.at)
 * Created      : 28/11/2007
 * Last Updated : 10/08/2008
 */

#ifndef _INNERSOLVER_CPP_
#define _INNERSOLVER_CPP_


#include "innersolver.hpp"
#include "utils/configuration.hpp"

InnerSolver::InnerSolver(Real lambda)
   : iter(0),  
     lambda(lambda), 
     dim(0), 
     numOfConstraint(0),
     nonNegativeSlack(true)
{
        // read configuration file
        
        assert(lambda > 0.0);
        
        
        // All CoFi losses are nonNegative 
        //    if(config.isSet("Loss.nonNegative"))  
        //		nonNegativeSlack = config.getIntAsBool("Loss.nonNegative");

}

#endif
