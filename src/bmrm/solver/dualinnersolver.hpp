
#ifndef _DUALINNERSOLVER_HPP_
#define _DUALINNERSOLVER_HPP_

#include "innersolver.hpp"
#include "utils/ublastools.hpp"

namespace DualSolver
{
        const double ZERO_EPS = 1e-20;
}


/** 
 *   when \Omega = 0.5|w|_2^2, the dual of the problem can be solved instead of the primal.
 */
class DualInnerSolver : public InnerSolver 
{
protected:
    
    /** Dimensionality of x vector before update with new gradient and removal of old gradients
     */
    int prevDim;

    /** The variables (i.e. Lagragian multipliers)
     */
    double *x;
    
    /** Linear part of the objective function
     */
    double *f;
    
    /** Hessian matrix of the objective function
     */
    double *Q;
    
    /** Constraint matrix
     */
    double *a;
    
    /** RHS of the (in)equality constraints
     */
    double *b;
    
    /** Lower bound vector for the variables
     */
    double *l;
    
    /** Upper bound vector for the variables
     */
    double *u;
    
    /** Tolerance for optimization error
     */
    double tol;
    
    /** Gradient set
     */
  std::vector<ublas::matrix<Real> >  gradientSet;
    
    /** Offsets set
     */
    std::vector<double> offsetSet;
    
    /** Type of gradient to be store in gradientSet (e.g., SPARSE or DENSE)
     *  [default: DENSE]
     */
    int gradType;
    
    /** Time stamps indicating the last iteration number a gradient is active.
     */
    std::vector<int> activeTimeStamp;

    /** Time stamps indicating the iteration number when a gradient entered the set
     */
    std::vector<int> enterTimeStamp;
    
    /** Max. number of consecutive iterations a Lagrange multiplier may
     *  remain zero before removal
     *  [default: 10]
     */
    int gradIdleAge;
    
    /** Maximum number of gradients to keep in gradientSet.
     *  Once the number of gradients exceeds this, oldest gradients 
     *  will be "aggregated" into one.
     *  [default: 1000]
     */
    int maxGradSetSize;
    
    /** Whether to remove ALL idling gradients exceeded gradIdleAge or just
     *  the oldest one.
     *  [default: false]
     */
    bool removeAllIdleGrad;
    
    /** Scaling factor for QP
     *  [default: 1.0]
     */
    double QPScale;
    
    /** index for aggregated gradient
     */
    int aggGradIdx;
    
    /** Solve QP
     */
    virtual void SolveQP()=0;
    
    
    /** Update the solver with new gradient
     */
    virtual void Update(const ublas::matrix<Real>& a, Real b);
    
    
    /** Remove ALL idle gradients and return a position for insertion of new gradient
     */
    virtual int RemoveAllIdleGradients();


    /** Remove the laziest idle gradient adn return a position for insertion of new gradient
     */
    virtual int RemoveLaziestIdleGradient();

    
    /** Aggregate gradients and return a position for insertion of new gradient
     */
    virtual int AggregateGradients();

        
//     /** Update QP data with new gradients at location #idx#
//      */
//     virtual void UpdateQP(int idx);


  /** Get solution and lower bound of empirical risk
   */
  virtual void GetSolution(ublas::matrix<Real>&  w, Real &objval);
    
    
#ifndef NDEBUG
  /** Routine to check the correctness of the quadratic matrix of the objective function
   */
  void MatrixCorrectnessCheck();
#endif
    
public:     
    
  /** Constructor
   */
  DualInnerSolver(Real lambda);      
  
    
  /** Destructor
   */
  virtual ~DualInnerSolver();
  
  
  /** Solve the problem
   */
  virtual void Solve(ublas::matrix<Real>& w, const ublas::matrix<Real>& grad, Real loss, Real &objval);
    

  /** Compute the value of regularizer
   *
   *  @param w [read] weight vector
   *  @return regularizer value
   */
  
  virtual double ComputeRegularizerValue(ublas::matrix<Real> &w);
    
    
  /** Reset the gradientSet, offsetSet, and mem for QP
   */
  virtual void Reset();
    
    
  /** With good QP tolerance annealing heuristic 
   *  the whole problem can be solved in much less number of iterations
   */
  virtual void SetTolerance(const Real &theTolerance) {tol = theTolerance;}
};

#endif
