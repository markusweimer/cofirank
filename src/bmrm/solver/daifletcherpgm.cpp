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
 * Created      : 16/11/2006
 * Last Updated : 10/08/2008
 */

#ifndef _DAIFLETCHERPGM_CPP_
#define _DAIFLETCHERPGM_CPP_

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>

#include "daifletcherpgm.hpp"

#define INFTY     1e30
#define ZERO_EPS  1e-16

DaiFletcherPGM::DaiFletcherPGM(double lambda)
   : DualInnerSolver(lambda),
     ipt(0),
     ipt2(0),
     uv(0),
     g(0),
     y(0),
     tempv(0),
     d(0),
     Qd(0),
     t(0),
     xplus(0),
     tplus(0),
     sk(0),
     yk(0)
{
   // default values
   tol = 1e-6;
   maxProjIter = 200; 
   maxPGMIter = 300000;

   // synchronize members with configuration file
   //   Configuration &config = Configuration::getInstance();
  
   // if(config.isSet("DaiFletcherPGM.maxProjIter"))
   //   maxProjIter = config.getInt("DaiFletcherPGM.maxProjIter");

   //if(config.isSet("DaiFletcherPGM.maxPGMIter"))
   //  maxPGMIter = config.getInt("DaiFletcherPGM.maxPGMIter");
   
//    if(config.IsSet("DaiFletcherPGM.tolerance")) 
//       tol = config.GetDouble("DaiFletcherPGM.tolerance");

   
   // mem space required by dai-fletcher 
   ipt   = (int  *)realloc(ipt,   maxGradSetSize*sizeof(int));
   ipt2  = (int  *)realloc(ipt2,  maxGradSetSize*sizeof(int));
   uv    = (int  *)realloc(uv,    maxGradSetSize*sizeof(int));
   g     = (double *)realloc(g,     maxGradSetSize*sizeof(double));
   y     = (double *)realloc(y,     maxGradSetSize*sizeof(double));
   tempv = (double *)realloc(tempv, maxGradSetSize*sizeof(double));
   d     = (double *)realloc(d,     maxGradSetSize*sizeof(double));
   Qd    = (double *)realloc(Qd,    maxGradSetSize*sizeof(double));
   t     = (double *)realloc(t,     maxGradSetSize*sizeof(double));
   xplus = (double *)realloc(xplus, maxGradSetSize*sizeof(double));
   tplus = (double *)realloc(tplus, maxGradSetSize*sizeof(double));
   sk    = (double *)realloc(sk,    maxGradSetSize*sizeof(double));
   yk    = (double *)realloc(yk,    maxGradSetSize*sizeof(double));

 
}

DaiFletcherPGM::~DaiFletcherPGM()
{
   // free mem allocated
   if(ipt)   free(ipt);
   if(ipt2)  free(ipt2);
   if(uv)    free(uv);
   if(g)     free(g);
   if(y)     free(y);
   if(tempv) free(tempv);
   if(d)     free(d);
   if(Qd)    free(Qd);
   if(t)     free(t);
   if(xplus) free(xplus);
   if(tplus) free(tplus);
   if(sk)    free(sk);
   if(yk)    free(yk);
}


/** Piecewise linear monotone target function for the Dai-Fletcher projector.
 *
 *  
 */
double DaiFletcherPGM::ProjectR(double *x1, int n, double lambda, double *a1, 
                                      double b, double *c, double *l, double *u)
{
   register int i;
   double r = 0.0;
   
   for (i = 0; i < n; i++)
   {
      x1[i] = -c[i] + lambda*a1[i];
      if (x1[i] > u[i]) 
         x1[i] = u[i];
      else if(x1[i] < l[i]) 
         x1[i] = l[i];
      r += a1[i]*x1[i];
   }
   
   return (r - b);
}




/** Modified Dai-Fletcher QP projector solves the problem:
 *
 *      minimise  0.5*x'*x + c'*x
 *      subj to   a'*x <= b
 *                l <= x <= u  
 *
 *  \param c The point to be projected onto feasible set
 */ 
int DaiFletcherPGM::ProjectDF(int n, double *a, double b, double *c, double *l, 
                                    double *u, double *x, double &lam_ext)
{
   double lambda, lambdal, lambdau, dlambda, lambda_new;
   double r, rl, ru, s;
   int    innerIter;
   
   lambda  = lam_ext = 0;
   dlambda = 0.5;
   innerIter = 1;
   
   /*  \phi(x;lambda) := 0.5*x'*x + c'*x - lambda*(a'*x-b)
    *
    *  Optimality conditions for \phi:
    *
    *  1. lambda   <= 0
    *  2. r        <= 0
    *  3. r*lambda == 0
    */
   
   
   // Bracketing Phase
   r = ProjectR(x, n, lambda, a, b, c, l, u);
   

   if(nonNegativeSlack)
   {
	   // chteo: inequality constraint, i.e., with \xi >= 0 constraint
	   if (r < DaiFletcher::tol_r) 
		   return 0;
   }
   else
   {
	   // chteo: equality constraint ,i.e., without \xi >= 0 constraint
	   if (fabs(r) < DaiFletcher::tol_r) 
		   return 0;
   }

   
   if (r < 0.0)
   {
      lambdal = lambda;
      rl      = r;
      lambda  = lambda + dlambda;
      r       = ProjectR(x, n, lambda, a, b, c, l, u);
      while (r < 0.0 && dlambda < INFTY)
      {
	 lambdal = lambda;
	 s       = rl/r - 1.0;
	 if (s < 0.1) s = 0.1;
	 dlambda = dlambda + dlambda/s;
	 lambda  = lambda + dlambda;
	 rl      = r;
	 r       = ProjectR(x, n, lambda, a, b, c, l, u);
      }
      lambdau = lambda;
      ru      = r;
   }
   else
   {
      lambdau = lambda;
      ru      = r;
      lambda  = lambda - dlambda;
      r       = ProjectR(x, n, lambda, a, b, c, l, u);
      while (r > 0.0 && dlambda > -INFTY)
      {
	 lambdau = lambda;
	 s       = ru/r - 1.0;
	 if (s < 0.1) s = 0.1;
	 dlambda = dlambda + dlambda/s;
	 lambda  = lambda - dlambda;
	 ru      = r;
	 r       = ProjectR(x, n, lambda, a, b, c, l, u);
      }
      lambdal = lambda;
      rl      = r;
   }
   
   if(fabs(dlambda) > INFTY) {
      printf("ERROR: DaiFletcherPGM detected Infeasible QP problem!\n");
      exit(0);
   }
   
   // Secant Phase
   s       = 1.0 - rl/ru;
   dlambda = dlambda/s;
   lambda  = lambdau - dlambda;
   r       = ProjectR(x, n, lambda, a, b, c, l, u);
   
   while (   fabs(r) > DaiFletcher::tol_r 
	     && dlambda > DaiFletcher::tol_lam * (1.0 + fabs(lambda)) 
	     && innerIter    < maxProjIter                    )
   {
      innerIter++;
      if (r > 0.0)
      {
	 if (s <= 2.0)
	 {
	    lambdau = lambda;
	    ru      = r;
	    s       = 1.0 - rl/ru;
	    dlambda = (lambdau - lambdal) / s;
	    lambda  = lambdau - dlambda;
	 }
	 else
	 {
	    s          = ru/r-1.0;
	    if (s < 0.1) s = 0.1;
	    dlambda    = (lambdau - lambda) / s;
	    lambda_new = 0.75*lambdal + 0.25*lambda;
	    if (lambda_new < (lambda - dlambda))
	       lambda_new = lambda - dlambda;
	    lambdau    = lambda;
	    ru         = r;
	    lambda     = lambda_new;
	    s          = (lambdau - lambdal) / (lambdau - lambda);
	 }
      }
      else
      {
	 if (s >= 2.0)
	 {
	    lambdal = lambda;
	    rl      = r;
	    s       = 1.0 - rl/ru;
	    dlambda = (lambdau - lambdal) / s;
	    lambda  = lambdau - dlambda;
	 }
	 else
	 {
	    s          = rl/r - 1.0;
	    if (s < 0.1) s = 0.1;
	    dlambda    = (lambda-lambdal) / s;
	    lambda_new = 0.75*lambdau + 0.25*lambda;
	    if (lambda_new > (lambda + dlambda))
	       lambda_new = lambda + dlambda;
	    lambdal    = lambda;
	    rl         = r;
	    lambda     = lambda_new;
	    s          = (lambdau - lambdal) / (lambdau-lambda);
	 }
      }
      r = ProjectR(x, n, lambda, a, b, c, l, u);
   }
   
   lam_ext = lambda;
   
   return (innerIter);
}


void DaiFletcherPGM::SolveQP()
{
   int    i, j, innerIter, it, luv, info, lscount = 0, projcount = 0;
   double gd, max, ak, bk, akold, bkold, lamnew, alpha, kktlam=0.0, lam_ext;
   double DELTAsv, ProdDELTAsv;
   double c;
   

   /*** variables for the adaptive nonmonotone linesearch ***/
   int    L, llast;
   double fr, fbest, fv, fc, fv0;


   c = INFTY;
   DELTAsv = DaiFletcher::EPS_SV;// * c;
   if (tol <= 1.0e-5 || dim <= 20)
      ProdDELTAsv = 0.0F;
   else
      ProdDELTAsv = DaiFletcher::EPS_SV;// * c;
   
   for (i = 0; i < dim; i++) {
      tempv[i] = -x[i];
   }
   
   lam_ext = 0.0;
   
   projcount += ProjectDF(dim, a, *b, tempv, l, u, x, lam_ext);
   
   // g = Q*x + f;
   // SparseProd(dim, t, Q, x, ipt);
   {
      int    it;
      double *tempQ;
      
      it = 0;
      for (i = 0; i < dim; i++)
	 if (fabs(x[i]) > ProdDELTAsv)
	    ipt[it++] = i;
      
      memset(t, 0, dim*sizeof(double));
      for (i = 0; i < it; i++)
      {
	 tempQ = Q + ipt[i] * dim;
	 for (j = 0; j < dim; j++)
	    t[j] += (tempQ[j]*x[ipt[i]]);
      }
   }
   
   for (i = 0; i < dim; i++) 
   {
      g[i] = t[i] + f[i];
      y[i] = g[i] - x[i];
   }
   
   projcount += ProjectDF(dim, a, *b, y, l, u, tempv, lam_ext);
   
   max = DaiFletcher::alpha_min;
   for (i = 0; i < dim; i++)
   {
      y[i] = tempv[i] - x[i];
      if (fabs(y[i]) > max)
          max = fabs(y[i]);
   }
   
   if (max < tol*1e-3)
   {
      lscount = 0;
      innerIter    = 0;
      goto Clean;    
   }
   
   alpha = 1.0 / max;
   
   fv0   = 0.0;
   for (i = 0; i < dim; i++)
      fv0 += x[i] * (0.5*t[i] + f[i]);
   
   /*** adaptive nonmonotone linesearch ***/
   L     = 2;
   fr    = DaiFletcher::alpha_max;
   fbest = fv0;
   fc    = fv0;
   llast = 0;
   akold = bkold = 0.0;
   
   for (innerIter = 1; innerIter <= maxPGMIter; innerIter++)
   {
      for (i = 0; i < dim; i++)
        tempv[i] = alpha*g[i] - x[i];
      
      projcount += ProjectDF(dim, a, *b, tempv, l, u, y, lam_ext);
      
      gd = 0.0;
      for (i = 0; i < dim; i++)
      {
        d[i] = y[i] - x[i];
        gd  += d[i] * g[i];
      }
      
      /* compute Qd = Q*d  or  Qd = Q*y - t depending on their sparsity */
      {
         int    i, it, it2;
         double *tempQ;
         
         it = it2 = 0;
         for (i = 0; i < dim; i++)
            if (fabs(d[i]) > (ProdDELTAsv*1.0e-2))
               ipt[it++]   = i;
         for (i = 0; i < dim; i++)
            if (fabs(y[i]) > ProdDELTAsv)
               ipt2[it2++] = i;
         
         memset(Qd, 0, dim*sizeof(double));
         if (it < it2) // compute Qd = Q*d
         {
            for (i = 0; i < it; i++)
            {
               tempQ = Q + ipt[i]*dim;
               for (j = 0; j < dim; j++)
                  Qd[j] += (tempQ[j] * d[ipt[i]]);
            }
         }
         else          // compute Qd = Q*y-t
         {
            for (i = 0; i < it2; i++)
            {
               tempQ = Q + ipt2[i]*dim;
               for (j = 0; j < dim; j++)
                  Qd[j] += (tempQ[j] * y[ipt2[i]]);
            }
            for (j = 0; j < dim; j++)
               Qd[j] -= t[j];
         }
      }
      
      ak = 0.0;
      for (i = 0; i < dim; i++)
         ak += d[i] * d[i];
      
      bk = 0.0;
      for (i = 0; i < dim; i++)
         bk += d[i]*Qd[i];
      
      if (bk > DaiFletcher::eps*ak && gd < 0.0)    // ak is normd
         lamnew = -gd/bk;
      else
         lamnew = 1.0;
      
      
      fv = 0.0;
      for (i = 0; i < dim; i++)
      {
         xplus[i] = x[i] + d[i];
         tplus[i] = t[i] + Qd[i];
         fv      += xplus[i] * (0.5*tplus[i] + f[i]);
      }
      
      if ((innerIter == 1 && fv >= fv0) || (innerIter > 1 && fv >= fr))
      {
         lscount++;
         fv = 0.0;
         for (i = 0; i < dim; i++)
         {
            xplus[i] = x[i] + lamnew*d[i];
            tplus[i] = t[i] + lamnew*Qd[i];
            fv      += xplus[i] * (0.5*tplus[i] + f[i]);
         }
      }
      
      for (i = 0; i < dim; i++)
      {
         sk[i] = xplus[i] - x[i];
         yk[i] = tplus[i] - t[i];
         x[i]  = xplus[i];
         t[i]  = tplus[i];
         g[i]  = t[i] + f[i];
      }
      
      // update the line search control parameters
      
      if (fv < fbest)
      {
         fbest = fv;
         fc    = fv;
         llast = 0;
      }
      else
      {
         fc = (fc > fv ? fc : fv);
         llast++;
         if (llast == L)
         {
            fr    = fc;
            fc    = fv;
            llast = 0;
         }
      }
      
      ak = bk = 0.0;
      for (i = 0; i < dim; i++)
      {
         ak += sk[i] * sk[i];
         bk += sk[i] * yk[i];
      }
      
      if (bk <= DaiFletcher::eps*ak)
         alpha = DaiFletcher::alpha_max;
      else
      {
         if (bkold < DaiFletcher::eps*akold)
	    alpha = ak/bk;
         else
            alpha = (akold+ak)/(bkold+bk);
         
         if (alpha > DaiFletcher::alpha_max)
            alpha = DaiFletcher::alpha_max;
         else if (alpha < DaiFletcher::alpha_min)
            alpha = DaiFletcher::alpha_min;
      }
      
      akold = ak;
      bkold = bk;
      
      
      /*** stopping criterion based on KKT conditions ***/
      // at optimal, gradient of lagrangian w.r.t. x is zero
      
      bk = 0.0;
      for (i = 0; i < dim; i++)
         bk +=  x[i] * x[i];
      
      
      if (sqrt(ak) < tol*10 * sqrt(bk))
      {
         it     = 0;
         luv    = 0;
         kktlam = 0.0;
         for (i = 0; i < dim; i++)
         {
            // x[i] is active hence lagrange multipliers for box constraints
            //        are zero. The lagrange multiplier for ineq. const. is then 
            //        defined as below
            
            if ((x[i] > DELTAsv) && (x[i] < c-DELTAsv))
            {
               ipt[it++] = i;
               kktlam    = kktlam - a[i]*g[i];
            }
            else
               uv[luv++] = i;
         }
         
         if (it == 0)
         {
            if (sqrt(ak) < tol*0.5 * sqrt(bk))
               goto Clean;
         }
         else
         {
            
            kktlam = kktlam/it;
            info   = 1;
            for (i = 0; i < it; i++) {
               if ( fabs(a[ipt[i]] * g[ipt[i]] + kktlam) > tol )
               {
                  info = 0;
                  break;
               }
            }
            if (info == 1)
               for (i = 0; i < luv; i++)
                  if (x[uv[i]] <= DELTAsv)
                  {
                     // x[i] == lower bound, hence, lagrange multiplier (say, beta) for lower bound may
                     //       not be zero. So, the gradient without beta is > 0
                     if (g[uv[i]] + kktlam*a[uv[i]] < -tol)
                     {
                        info = 0;
                        break;
                     }
                  }
                  else
                  {
                     // x[i] == upper bound, hence, lagrange multiplier (say, eta) for upper bound may
                     //       not be zero. So, the gradient without eta is < 0
                     if (g[uv[i]] + kktlam*a[uv[i]] > tol)
                     {
                        info = 0;
                        break;
                     }
                  }
            
            if (info == 1)
               goto Clean;
         }
      }
   }
   
   
  Clean:;
   // Verbosity removed
}

#endif
