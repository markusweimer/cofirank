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
 *                Alexandros Karatzoglou (alexis@ci.tuwien.ac.at)
 * Created      : 25/07/2007
 * Last Updated : 10/08/2008
 */

#ifndef _INNERSOLVER_HPP_
#define _INNERSOLVER_HPP_

#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include "core/types.hpp"


/** 
 * Abstract class for representing an inner solver which solves the
 * following type of mathematcal program:  
 *
 *  minimise  \lambda \Omega(w) + \xi
 *     w
 *
 *  s.t.      <w-w_i, \partial_w R(w_i)> + R(w_i) < \xi \forall i
 *            \xi > 0 [optional]
 *
 *  where     \Omega is regularizer e.g., 1/2 |w|_2^2
 *            \lambda is regularization constant
 *            R(w) is a convex, non-negative risk function
 *
 */
class InnerSolver
{  
protected:
 
    /** Number of times inner solver has been called
     */
    int iter;
    
    /** Regularization constant
     */
	double lambda;
      
    /** Dimensionality of the problem
     */
    int dim;
    
    /** Number of constraints
     */
    int numOfConstraint;
    
  
	/** Positive slack constaint
	 */
	bool nonNegativeSlack;
    
public:
    /** Constructors
     */
    InnerSolver(Real lambda);
    
	
    /** Destructor
     */
    virtual ~InnerSolver(){}
    
    
    /** With good QP tolerance annealing heuristic (e.g., in svmperf),
     *  the whole problem can be solved in much less number of iterations
     */
    virtual void SetTolerance(const Real &theTolerance) {}
    
    
    /** Solve the problem
     *
     *  @param w [r/w] Weight vector
     *  @param grad [read] gradient vector
     *  @param loss [read] loss function value
     *  @param xi [write] approximate to the loss
     *  @param regval [write] regularization term
     *  @param objval [write] objective value of the piece-wise linear lower bound
     *  @param maxtol [read] maximum tolerance for the inner solver (applicable for some inner solvers only)
     */
  virtual void Solve(ublas::matrix<Real>& w, const ublas::matrix<Real>& grad, Real loss, Real &objval)=0; 
	/** Compute the value of the regularizer
	 *
	 *  @param w [read] weight vector
	 *  @return the regularizer value
	 */
	virtual double ComputeRegularizerValue(ublas::matrix<Real> &w)=0;
    
    /** Reset the the inner solver for a fresh start         
     *  Meaningful for those solvers which store past gradients
     */
    virtual void Reset(){};
};

#endif
