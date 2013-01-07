/* 
 * Purpose      : L1-norm Linear Program solver using COIN-OR Clp 
 *
 */

#ifndef _L1N1_CLP_CPP_
#define _L1N1_CLP_CPP_

#include "l1n1_clp.hpp"
#include <iostream>
#include "utils/configuration.hpp"
#include "utils/ublastools.hpp"


// Clp headers
#include "coin-clp/include/CoinBuild.hpp"

#define INFTY     1e30
#define ZERO_EPS  1e-16


using namespace std;

/*
    L1-Norm linear program:

    minimize   :  d'x_bar
     x_bar
     
    subject to : | -1_cm  A_mn  -A_mn | |\xi|    
                                        | v | <= |-B| 
                                        | u |    


    where      : d := [1; lambda; -lambda] (length: [1,n,n])
                 C is a positive scalar
                 lambda is a column vector of size n
                 x_bar := [\xi; v; u]
                 u,v column vectors of size n
                 1_c# is a column vector of 1 of size #,
                 A_#* is a dense matrix of size #-by-*, and
                 m << n (m starts from 1 and increase by 1 after 
                 each lp optimization round.)
				 B is a column vector of size m

*/


L1N1_Clp::L1N1_Clp(double lambda, const int &thedim)
   : InnerSolver(lambda),
     newRowIndices(0),
     newRowElements(0),
     gradIdleAge(10),
     removeAllIdleGrad(false)
{
  // parameters
  iter = 0;
  dim = thedim;
  numOfConstraint = 0;

  // sanity check
  assert(dim > 0);
  
  // read configuration
  Configuration &config = Configuration::getInstance();
  if(config.isSet("L1N1_Clp.gradIdleAge"))
  {
      gradIdleAge = config.getInt("L1N1_Clp.gradIdleAge");
      gradIdleAge = max(2,gradIdleAge);
  }
  
  if(config.isSet("L1N1_Clp.removeAllIdleGradients"))
      removeAllIdleGrad = config.getIntAsBool("L1N1_Clp.removeAllIdleGradients");

  // build simplex model
  sim = new ClpSimplex();
  sim->setLogLevel(0);
  sim->resize(0, 1+2*dim);

  // set objective coefficients    
  sim->setObjCoeff(0, 1.0);
  for(int i=1; i<=dim; i++) 
     sim->setObjCoeff(i, lambda);
  for(int i=dim+1; i<=2*dim; i++)
     sim->setObjCoeff(i, -lambda);

  // set bounds for variables (i.e., columns as in COIN Clp terminology)
  if(nonNegativeSlack)
	  sim->setColumnBounds(0, 0.0, COIN_DBL_MAX);
  else
	  sim->setColumnBounds(0, -COIN_DBL_MAX, COIN_DBL_MAX);  

  for (int i=1; i<2*dim+1; i++)
     sim->setColumnBounds(i, 0.0, COIN_DBL_MAX);
  
  
  // something useful in constraint matrix update phase
  newRowIndices = (int*)calloc(2*dim+1, sizeof(int));
  newRowElements = (double*)calloc(2*dim+1, sizeof(double));    
  
  for(int i=0; i<2*dim+1; i++)
     newRowIndices[i] = i;
  
  newRowElements[0] = -1.0;
  // Update() set the rest of the elements in newRowElements[]
    
}


L1N1_Clp::~L1N1_Clp()
{        
    if(sim) delete sim;
    if(newRowIndices) free(newRowIndices);
    if(newRowElements) free(newRowElements);  
}


void L1N1_Clp::Solve(ublas::matrix<Real>& w, const ublas::matrix<Real>& grad, Real loss, Real &objval)
{

    double w_dot_grad = cofi::ublastools::inner_prod(w, grad);
    Update(grad, loss - w_dot_grad);
    //sim->dual();
    sim->primal();
    GetSolution(w, objval);
}


/** Clear constraint matrix
 */
void L1N1_Clp::Reset()
{
    // delete rows in constraint matrix
    int nrows = sim->numberRows();
    int *rows = new int[nrows];
    for(int i=0; i<nrows; i++) 
        rows[i] = i;
    sim->deleteRows(nrows, rows);      
    delete rows;
}


/**  Update matrix and vectors needed in optimisation round.
 *
 *   \param a [read] Gradient
 *   \param b [read] Offset 
 */
void L1N1_Clp::Update(const ublas::matrix<Real>& a, const Real b)
{
    iter++;

    unsigned int length = a.size1();
    unsigned int width = a.size2();
    

    std::cout <<" Dim : "<< dim <<" \n width : " << width << " \n length : " << length << std::endl;
    assert(length*width == dim);
 
   
    // reset the content of newRowElements and newRowIndices
    memset(newRowElements, 0, sizeof(double)*(1+dim+dim));
    memset(newRowIndices, 0, sizeof(int)*(1+dim+dim));
    newRowElements[0] = -1;
    newRowIndices[0] = 0;
    
    double val = 0;
    unsigned int nnz = 0;
 
    if(width == 1) 
      for(unsigned int i=0; i<length; i++)  
	{
	  val= a(i,0); 
	  if(abs(val) > ZERO_EPS) 
	    {
	      newRowElements[1+nnz] = val;
	      newRowIndices[1+nnz] = i + 1;  // gradient starts from second column
	      nnz++;
	    }
	}
    else
      for(unsigned int i=0; i<length; i++)  
	for(unsigned int j=0; j < width; j++)  
	  {
	    val= a(i,j); 
	    if(abs(val) > ZERO_EPS) 
	      {
		newRowElements[1+nnz] = val;
		newRowIndices[1+nnz] = i*width + j + 1;  // gradient starts from second column
		nnz++;
	      }
	  }
    
    for(unsigned int i=0; i<nnz; i++)  
    {
      newRowElements[1+nnz+i] = -newRowElements[1+i];
      newRowIndices[1+nnz+i]  = newRowIndices[1+i]+dim;
    }
    
    sim->addRow(1+nnz+nnz, newRowIndices, newRowElements, -COIN_DBL_MAX, -b);
    numOfConstraint++;
    activeTimeStamp.push_back(iter);
    
}


double L1N1_Clp::ComputeRegularizerValue(ublas::matrix<Real>& w)
{
	double regval = 0.0;
	regval = norm_1(w);
	regval *= lambda;
	return regval;
}


/** Return solution.
 *
 *  \param w      [write] solution vector (preallocated)
 *  \param objval [write] objective value
 */
void L1N1_Clp::GetSolution(ublas::matrix<Real>& w, Real &objval)
{
    double *solution = 0;
    solution = sim->primalColumnSolution(); 
    assert(solution);
    
    // construct new w
    //AK Note : we need to take care of the case where w is a matrix (see moviephase)
    unsigned int length = w.size1();
    unsigned int width = w.size2();

    if(width==1)
      for(unsigned int i=0; i<length; i++) 
	{
	  w(i,0) = solution[1+i] - solution[1+dim+i];   // w := u-v
	}
    else
      for(unsigned int i=0; i<length; i++) 
	for(unsigned int j=0; j<width; j++)
	  {
	    w(i,j) = solution[1+i*width +j] - solution[1+dim+i*width +j];   // w := u-v
	  }
    
    objval = sim->objectiveValue();
    
    vector<int> del;
    int nrows = sim->numberRows();

    if(removeAllIdleGrad)
    {
        for(int i=0; i<nrows; i++)
        {
            if(sim->getRowStatus(i)==ClpSimplex::atUpperBound)
                activeTimeStamp[i] = iter;
            
            if(iter-activeTimeStamp[i] >= gradIdleAge)

                del.push_back(i);
        }
        
        
        sim->deleteRows(del.size(), &(del[0]));
        int size=del.size();
        for(int i=size-1; i>0; i--)
            activeTimeStamp.erase(activeTimeStamp.begin() + del[i]);
        numOfConstraint -= del.size();
    }
    else
    {
        int oldestTS = activeTimeStamp[0];
        int oldestIdx = 0;
        int nrows = sim->numberRows();
        
        for(int i=0; i<nrows; i++)
        {
            if(sim->getRowStatus(i)==ClpSimplex::atUpperBound)
                activeTimeStamp[i] = iter;
            
            if(activeTimeStamp[i] < oldestTS)
            {
                oldestTS = activeTimeStamp[i];
                oldestIdx = i;
            }
        }
        
        // delete oldest inactive constraint
        if(iter-oldestTS >= gradIdleAge)
        {
            sim->deleteRows(1,&oldestIdx);
            activeTimeStamp.erase(activeTimeStamp.begin() + oldestIdx);
            numOfConstraint--;
        }
    }
}

#endif
