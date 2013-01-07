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
 * Created      : 20/11/2007
 * Last Updated : 10/08/2008
 */

#ifndef _DUALINNERSOLVER_CPP_
#define _DUALINNERSOLVER_CPP_

#include <fstream>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>

#include "utils/ublastools.hpp"
#include "dualinnersolver.hpp"
#include "utils/configuration.hpp"

#define AGG_GRAD_TIME_STAMP 99999
#define INFTY     1e30
#define ZERO_EPS  1e-16

// in general, BMRM needs more iterations to converge if we remove all IDLE gradients at once

DualInnerSolver::DualInnerSolver(Real lambda)
   : InnerSolver(lambda),
     prevDim(0),
     f(0),
     Q(0),
     a(0),
     b(0),
     l(0),
     u(0),
     tol(1e-6),
     removeAllIdleGrad(false),
     QPScale(1.0),
     aggGradIdx(-1)
{
   // inherited from CInnerSolver
   iter = 0;
   dim = 0;
   numOfConstraint = 1;
   
   // set (default) private member values
   gradIdleAge = 9;
   maxGradSetSize = 100;  // max num of gradients
   
   //AK Note : we should pass this options from cofi

   // synchronize members with configuration file
   //  Configuration &config = Configuration::getInstance();

   //   if(config.isSet("DualInnerSolver.maxGradSetSize"))
   //       maxGradSetSize = config.getInt("DualInnerSolver.maxGradSetSize");
   
   //    if(config.isSet("DualInnerSolver.gradIdleAge")) 
   //    {
   //        gradIdleAge = config.getInt("DualInnerSolver.gradIdleAge");
   //        gradIdleAge = max(gradIdleAge, 2);
   //    }
   
   //    if(config.isSet("DualInnerSolver.removeAllIdleGradients"))
   //        removeAllIdleGrad = config.getIntAsBool("DualInnerSolver.removeAllIdleGradients");

   
   // pre-allocate memory for gradient, offset and active and enter time-stamps
   gradientSet.resize(maxGradSetSize);                  
   offsetSet.resize(maxGradSetSize,0);                  
   activeTimeStamp.resize(maxGradSetSize,0);  
   enterTimeStamp.resize(maxGradSetSize,0);   
   
   // allocate maximum memory needed
   x = (double*)calloc(maxGradSetSize, sizeof(double));
   Q = (double*)calloc(maxGradSetSize*maxGradSetSize, sizeof(double));
   f = (double*)calloc(maxGradSetSize, sizeof(double));
   l = (double*)calloc(maxGradSetSize, sizeof(double));
   u = (double*)calloc(maxGradSetSize, sizeof(double));
   a = (double*)calloc(maxGradSetSize, sizeof(double));
   b = new double(1.0); 
   
   // initializations
    Reset();
}


DualInnerSolver::~DualInnerSolver()
{
    // free for parent
    if(x) free(x);
    if(Q) free(Q);
    if(f) free(f);
    if(l) free(l);
    if(u) free(u);
    if(a) free(a);
    if(b) delete b;
    
    // Original
    //    for(int i=0; i < dim; i++)
    //    if(gradientSet[i])
    //        delete gradientSet[i];   
    
    // AK Note : We have to check if this is ok !
    for(int i=0; i < dim; i++)
      gradientSet[i].resize(0,0);
    // gradientSet[i].assign_temporary(ublas::matrix<Real>(0,0));
}


void DualInnerSolver::Reset()
{
    // QP mem and gradient related mem
    for(int i=0; i<maxGradSetSize; i++) 
    {
        x[i] = 0;
        f[i] = 0;                
        l[i] = 0;
        u[i] = 1.0;
        a[i] = 1.0;      
        // AK Note : We have to check if this is ok !
	//gradientSet[i].assign_temporary(ublas::matrix<Real>(0,0));
	gradientSet[i].resize(0,0);
	// Original
	//        if(gradientSet[i]) 
        //{
        //    delete gradientSet[i];
        //    gradientSet[i] = 0;
        //}
        offsetSet[i] = 0.0;
        activeTimeStamp[i] = 0;
        enterTimeStamp[i] = 0;
    }                
    memset(Q, 0, sizeof(double)*maxGradSetSize*maxGradSetSize); 
    
    // variables
    iter = 0;
    dim = 0;
}


void DualInnerSolver::Update(const ublas::matrix<Real>& a, const Real b)
{
    iter++;
    int idx = 0;
    prevDim = dim;

    if(removeAllIdleGrad)
        idx = RemoveAllIdleGradients();
    else
        idx = RemoveLaziestIdleGradient();
        
    
    if(dim > maxGradSetSize)
    {
        dim = maxGradSetSize;
        idx = AggregateGradients();
    }
    else if(prevDim < dim)
    {
        // adjust memory area to fit a new column, if needed
        int size = sizeof(double)*(dim-1);
        for(int row=dim-2; row >= 1; row--)
        {
            double *src  = Q + (row*(dim-1));
            double *dest = Q + (row*(dim-1))+ row;
            memmove(dest, src, size);
        }
    }

    prevDim = dim;
    // AK Note : We need to check if this is ok !
    //if(gradientSet[idx]) 
    //    delete gradientSet[idx];
    if(offsetSet[idx])
      gradientSet[idx].resize(0,0);
    gradientSet[idx] =  a;

    offsetSet[idx] = b;
    
    // update time stamps
    enterTimeStamp[idx] = iter;
    activeTimeStamp[idx] = iter;  

    // update QP hessian matrix and vectors

    // Insert row into Q
    int theRow = idx*dim;
    for(int i=0; i < dim; i++) 
    {
      double value = cofi::ublastools::inner_prod(gradientSet[idx], gradientSet[i]);
      Q[theRow + i] = value*QPScale/lambda;
    }
      
    // insert new column into Q[:,idx]
    for(int i=0; i < dim; i++)
    {
        Q[i*dim + idx] = Q[theRow + i];
    }
    
    // update vectors      
    x[idx] = 0;
    f[idx] = -offsetSet[idx]*QPScale;


#ifndef NDEBUG
    // Q matrix update correctness check!    
    MatrixCorrectnessCheck();
#endif  

}



int DualInnerSolver::AggregateGradients()
{
    // if aggregated gradient (AG) does not exist, let the earliest gradient be it
    if(aggGradIdx == -1)
    {
        aggGradIdx = 0;
        for(int i=0; i<dim; i++)
        {
            if(enterTimeStamp[aggGradIdx] > enterTimeStamp[i])
            {
                aggGradIdx = i;
            }
        }
        activeTimeStamp[aggGradIdx] = AGG_GRAD_TIME_STAMP;
    }
    // if AG had been moved to somewhere, find it
    else if(activeTimeStamp[aggGradIdx] != AGG_GRAD_TIME_STAMP)
    {
        for(int i=0; i<dim; i++)
            if(activeTimeStamp[i] == AGG_GRAD_TIME_STAMP)
            {
                aggGradIdx = i;
                break;
            }
    }
    // note that AG will almost never be deleted due to the large value of AGG_GRAD_TIME_STAMP

    
    // find most laziest/earliest gradient besides the agg. grad.
    int t1 = 0;  // for earliest gradient
    int t2 = 0;  // for laziest gradient
    int t3 = 0;  // for most active gradient
    
    t1 = t2 = t3 = (aggGradIdx+1)%dim;
    
    for(int i=0; i<dim; i++)
    {
        if(i == aggGradIdx) continue;
        t1 = (enterTimeStamp[t1] > enterTimeStamp[i] || t1 > i) ? i : t1;
        t2 = (activeTimeStamp[t2] > activeTimeStamp[i]) ? i : t2;
        t3 = (activeTimeStamp[t3] < activeTimeStamp[i]) ? i : t3;
    }
    
    // if laziest is not that lazy, we pick the earliest
    int idx = (t2 == t3) ? t1 : t2;

    assert(idx != aggGradIdx);
    if(idx == aggGradIdx) { std::clog << "idx == aggGradIdx" << std::endl; exit(EXIT_FAILURE); }
    
    // aggregation of gradients
    
    if(fabs(x[aggGradIdx]) < ZERO_EPS)
      { //AK Note : we need to check this !
        // swap aggGradIdx with idx
        //if(gradientSet[aggGradIdx]) 
      //   delete gradientSet[aggGradIdx];
	if(offsetSet[aggGradIdx])
	  gradientSet[aggGradIdx].resize(0,0);
        gradientSet[aggGradIdx] = gradientSet[idx];
        
	//gradientSet[idx] = 0;
	gradientSet[idx].resize(0,0);
        
	offsetSet[aggGradIdx] = offsetSet[idx];
        offsetSet[idx] = 0;

        x[aggGradIdx] = x[idx];
    }
    else if(fabs(x[aggGradIdx]) > ZERO_EPS && fabs(x[idx] > ZERO_EPS))
      {
	gradientSet[aggGradIdx] *= x[aggGradIdx];
	gradientSet[idx] *= x[idx];      
        gradientSet[aggGradIdx] += gradientSet[idx];
        gradientSet[aggGradIdx] *= 1.0/(x[aggGradIdx]+x[idx]);

        offsetSet[aggGradIdx] *= x[aggGradIdx];
        offsetSet[aggGradIdx] += x[idx]*offsetSet[idx];
        offsetSet[aggGradIdx] /= (x[aggGradIdx]+x[idx]);

        x[aggGradIdx] = 0.0;
    }
    // else, x[idx] == 0, do nothing
    
    // update the hessian matrix and vectors
    for(int i=0; i<dim; i++)
      {
        // note that the entries Q[aggGradIdx][idx] and Q[idx][aggGradIdx] will be replaced correctly 
        //  when new gradient take the position #idx#
        if(i == idx) continue;
        
        double value = cofi::ublastools::inner_prod(gradientSet[aggGradIdx], gradientSet[i]);
        Q[aggGradIdx*dim + i] = value*QPScale/lambda;
        Q[i*dim + aggGradIdx] = Q[aggGradIdx*dim + i];
      }
    
    f[aggGradIdx] = -offsetSet[aggGradIdx]*QPScale;     
    return idx;
}


/** Remove ALL idle gradients, if possible.
 *  Side effect:
 *  1.  change dim
 */
int DualInnerSolver::RemoveAllIdleGradients()
{
    int first_idle = 0;
    int last_idle = dim-1;
    int removeCnt = 0;

    // active gradients float to the top
    while(first_idle < last_idle)
    {                
        // look for the bottom most to-keep gradient and remove bottom most to-remove gradients
        while((last_idle > 0) && (iter-activeTimeStamp[last_idle] >= gradIdleAge)) 
        {
            if(offsetSet[last_idle]) 
	      {// AK Note : we need to check this !
	      //delete gradientSet[last_idle];
	      //gradientSet[last_idle] = 0;                                
		gradientSet[last_idle].resize(0,0);
		offsetSet[last_idle] = 0;
                x[last_idle] = 0;
                f[last_idle] = 0; 
                activeTimeStamp[last_idle] = 0;
                enterTimeStamp[last_idle] = 0;
            }                        
            removeCnt++;
            last_idle--;
        }
        
        // look for top most to-remove gradient
        while((first_idle < dim) && (iter-activeTimeStamp[first_idle] < gradIdleAge)) 
            first_idle++;
        
        // replace the top most to-remove gradient with the bottom most to-keep gradient
        if(first_idle < last_idle)
        {       
            // 1. remove/replace the elements in gradientSet, offsetSet, x, f, activeTimeStamp                        
          // AK Note : We need to check this !
	  //if(gradientSet[first_idle]) 
	  //    delete gradientSet[first_idle];
	  if(offsetSet[first_idle])
	    gradientSet[first_idle].resize(0,0);
	  gradientSet[first_idle] = gradientSet[last_idle];
            //gradientSet[last_idle] = 0;
	    //gradientSet[last_idle].clear();
	    //gradientSet[last_idle].resize(0,0);
	     
            offsetSet[first_idle] = offsetSet[last_idle];
            offsetSet[last_idle] = 0;
            
            x[first_idle] = x[last_idle];
            x[last_idle] = 0;
            
            f[first_idle] = f[last_idle];                                                     
            f[last_idle] = 0;
            
            activeTimeStamp[first_idle] = activeTimeStamp[last_idle];
            activeTimeStamp[last_idle] = 0;

            enterTimeStamp[first_idle] = enterTimeStamp[last_idle];
            enterTimeStamp[last_idle] = 0;
            
            // 2. memmove row                
            memmove(Q+first_idle*dim, Q+last_idle*dim, sizeof(double)*dim);
            
            // 3. memmove column
            for(int i=0; i<last_idle; i++)
                Q[i*dim + first_idle] = Q[i*dim + last_idle];                                
            
            // 4. one more to-remove gradient sink to the bottom
            removeCnt++;
        }
    }
    
    // make the hessian matrix compact
    if(removeCnt > 0)
    {
        assert(dim >= removeCnt);
        
        for(int i=1; i<dim; i++)
            memmove(Q+i*(dim-removeCnt), Q+i*dim, sizeof(double)*(dim-removeCnt));
        dim -= removeCnt;
    }
    dim++;
    
    return dim-1;  // i.e., use new slot
}


/** Remove the laziest gradient, if possible.
 *  Side effect:
 *  1. change dim
 */
int DualInnerSolver::RemoveLaziestIdleGradient()
{
    int idx = 0;

    for(int i=0; i < dim; i++) 
        idx = (activeTimeStamp[idx] > activeTimeStamp[i]) ? i : idx;
    
    // gradient not old enough
    if(iter - activeTimeStamp[idx] < gradIdleAge) 
    {
        idx = dim;
        dim++;
    }

    return idx;
}



/** Compute the value of regularizer
 *
 *  @param w [read] weight vector
 *  @return regularizer value
 */
double DualInnerSolver::ComputeRegularizerValue(ublas::matrix<Real> &w)
{
  double regval = ublas::norm_frobenius(w);
  regval = regval * regval * lambda * 0.5; 
  return regval;
}


/** Return solution.
 *
 *  \param w      [write] solution vector (preallocated)
 *  \param objval [write] objective value
 */
void DualInnerSolver::GetSolution(ublas::matrix<Real>& w, Real &objval)
{
    assert(x != 0);
    double factor = 1.0/lambda;  // lambda is regularization constant

    double threshold = ZERO_EPS;
    
    // compute objective value (explicitly)
    // n := dimensionality of x (the solution of QP)
    // Q := hessian matrix
    // f := linear part of obj
    
    objval = 0.0;
    double tmp = 0.0;
    double fx = 0.0;
    
    for(int i=0; i < dim; i++) 
    {
        tmp = 0;
        if(x[i] > threshold)
        {
            for(int j=0; j < dim; j++)
                if(x[j] > threshold)
                    tmp += Q[j + i*dim]*x[j];
            objval += x[i]*tmp;
            fx += f[i]*x[i];
        }
    }        
    
    // since the dual of minimization problem is maximization
    // and we solved the negated maximization version,
    // so the objective value should be -objval
    objval = -0.5*objval - fx;
    objval /= QPScale;
    

    // Compute new w
    w.clear();
    for(int i=0; i < dim; i++)
      if(x[i] > threshold)
	w += -x[i] * gradientSet[i];
	 
    w *= factor;


    // update time-stamp of active gradients
    for(int i=0; i < dim; i++)
        if(x[i] > threshold && activeTimeStamp[i] <= iter)
            activeTimeStamp[i] = iter;         

//#ifndef NDEBUG
//    // count nonzero terms
//        double smallest = 1e10;
//        double sum_x = 0.0;
//        int smallest_idx = -1;
//        int howmany = 0;
//
//        for(int i=0; i < dim; i++) 
//        {
//            if(x[i]> threshold) 
//            {
//                howmany++;
//                if(smallest > x[i])
//                {
//                    smallest = x[i];
//                    smallest_idx = i;
//                }
//                sum_x += x[i];
//            }
//        }
//
//
//        std::clog << "qp size =  " << dim << std::endl;
//        std::clog << "nnz(x) = " << howmany << std::endl;
//        std::clog << "arg,min x = " << smallest_idx << ", " << smallest << std::endl << std::endl;
//#endif
    
}


void DualInnerSolver::Solve(ublas::matrix<Real>& w, const ublas::matrix<Real>& grad, Real loss, Real &objval)
{
    double w_dot_grad = cofi::ublastools::inner_prod(w, grad);
    Update(grad, loss - w_dot_grad);
    SolveQP();
    GetSolution(w, objval);
}


/** Check if the Q matrix update is correct
 */

#ifndef NDEBUG
void DualInnerSolver::MatrixCorrectnessCheck() 
{
    //std::clog << "in Q matrix correctness check! " << endl;
    double *correctmat = new double[dim*dim];
    memset(correctmat, 0, sizeof(double)*dim*dim);
    
    for(int i=0; i < dim; i++)
    {
        for(int j=i; j < dim; j++)
        {
	  double value = cofi::ublastools::inner_prod(gradientSet[i],gradientSet[j]);
	  correctmat[i*dim + j] = QPScale*value/lambda;
	  correctmat[j*dim + i] = correctmat[i*dim+j];
        }
    }
    
    for(int i=0; i < dim*dim; i++)
    {
        if(fabs(correctmat[i] - Q[i]) > 1e-15) 
        {
            std::clog << "residual : " << fabs(correctmat[i] - Q[i]) << std::endl;
            std::clog << "Q matrix update correctness check : i = " << i << ", correct Q[i] = " << correctmat[i] << std::endl;
            std::clog << "ERROR: Q matrix is incorrect ! " << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    
    if(correctmat) 
        delete [] correctmat;
}
#endif

#endif
