
/* 
 * Purpose      : L1-norm Linear Program solver using COIN-OR Clp 
 *
 */

#ifndef _L1N1_CLP_HPP_
#define _L1N1_CLP_HPP_

#include "innersolver.hpp"
#include <vector>

// from COIN-OR Clp
#include "coin-clp/include/ClpSimplex.hpp"

/* 
 * solve the 1-norm linear program
 *
 * minimize    c' * x + |x|_1
 * subject to  r <= A*x <= r+b
 *             l <= x <= u
 *
 */
class L1N1_Clp : public InnerSolver 
{
protected:
    /** CLP simplex solver
     */
    ClpSimplex *sim;
    
    /** Pre-allocated indices array for new constaint
         */
    int *newRowIndices;
    
    /** Pre-allocated values array for new constraint
     */
    double *newRowElements;

    /** Latest time stamp for a constraint being active (i.e. getRowStatus(i) == ClpSimpex::atUpperBound)
     */
    std::vector<int> activeTimeStamp;

    /** Max. number of consecutive iterations a constraint may
     *  remain inactive (i.e. getRowStatus(i) == ClpSimpex::basic)
     *  [default: 10]
     */
    int gradIdleAge;

    /** If true then removel all inactive constraint (or gradient),
     *  otherwise, remove the oldest one.
     *  [default: false]
     */
    bool removeAllIdleGrad;
    
    /** Update the constraint matrix
     */
    virtual void Update(const ublas::matrix<Real>& a, const Real b);
    
    /** Return the updated solution
     */
    virtual void GetSolution(ublas::matrix<Real>& w, Real &objval);

public:      
    
    /** Constructor
     */
    L1N1_Clp(double lambda, const int &thedim);
    
    /** Destructor
     */
    virtual ~L1N1_Clp();
    
    /** Solve the problem
     */
    virtual void Solve(ublas::matrix<Real>& w, const ublas::matrix<Real>& grad, Real loss, Real &objval);
    
	/** Compute the value of the regularizer
	 *
	 *  @param w [read] weight vector
	 *  @return the regularizer value
	 */
	virtual double ComputeRegularizerValue(ublas::matrix<Real>& w);
    
    
    /** Clear constraint matrix
     */
    virtual void Reset();
};

#endif
