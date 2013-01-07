#ifndef _BMRM_CPP_
#define _BMRM_CPP_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "bmrm.hpp"
#include "lossfunction.hpp"
#include "utils/configuration.hpp"
#include "core/cofiexception.hpp"
#include "solver/innersolver.hpp"

/**  
 *  Constructor
 *
 *  @param model [read] pointer to loss model object
 */
BMRM::BMRM(LossFunction& lossFunction, const Real lambda, const size_t dimW) : lossFunction(lossFunction), lambda(lambda), dimW(dimW) {
    // set private members (default) values
    maxNumOfIter = 100;
    epsilonTol = 0.1;
    relEpsilonTol = 0.02;
    gammaTol = 0.1;
    relGammaTol = 0.02;


    // instantiate inner solver
    innerSolver = new DaiFletcherPGM(lambda);
}

/**  Destructor
 */
BMRM::~BMRM() {
    // destroy inner solver
    if (innerSolver) {
        delete innerSolver;
        innerSolver = 0;
    }
}

/**  Start training/learning a model w.r.t. the loss object (and the data supplied to it).
 *
 */
Real BMRM::train(ublas::matrix<Real>& w) {
    unsigned int iter = 0; // iteration count
    double loss = 0.0; // loss function value        
    double approxObjVal = -std::numeric_limits<Real>::infinity(); // convex lower-bound (approximate) of objective function value
    double minExactObjVal = std::numeric_limits<Real>::infinity(); // minimum of all previously evaluated (exact) objective function value
    double epsilon = 0.0; // := minExactObjVal - approxObjVal       
    double prevEpsilon = 0.0;
    double innerSolverTol = 1.0; // optimization tolerance for inner solver
    double finalExactObjVal = 0; // minimum of all previously evaluated exact obj except the first iteration
    double finalLoss = 0.0;
    double finalRegVal = 0.0;

    ublas::matrix<Real> w_final(w.size1(), w.size2()); // w_t at which pobj is the smallest (t>=2, i.e., initial w is not considered)
    ublas::matrix<Real> gradient(w.size1(), w.size2());
    #ifndef NDEBUG
    std::clog << "gammaTol: " << gammaTol << " epsilonTol: " << epsilonTol << " relGammaTol: " << relGammaTol << " relEpsilonTol: "<< relEpsilonTol << " maxIter: " << maxNumOfIter << std::endl;
    #endif
    while (1) {
        iter++;
       

        // column generation
        lossFunction.ComputeLossGradient(w, loss, gradient);

        assert(gradient.size1() == w.size1() && gradient.size2() == w.size2());
        assert(loss >= 0.0);
        // update convergence monitor
        const double regVal = innerSolver->ComputeRegularizerValue(w); // value of the regularizer term e.g., 0.5*w'*w
        const double exactObjVal = loss + regVal; // (exact) objective function value
        minExactObjVal = std::min<Real > (minExactObjVal, exactObjVal);

        if (iter == 2) {
            finalExactObjVal = exactObjVal;
            finalLoss = loss;
            finalRegVal = regVal;
            w_final = w;
        } else if (iter > 2) {

            if (finalExactObjVal > exactObjVal) {
                finalExactObjVal = exactObjVal;
                finalLoss = loss;
                finalRegVal = regVal;
                w_final = w;
            }
        }

        prevEpsilon = epsilon;
        epsilon = minExactObjVal - approxObjVal;
        const double gamma = exactObjVal - approxObjVal;



        // stopping criteria
        if (iter >= maxNumOfIter) {
            std::clog << "\nWARNING: BMRM exceeded maximum number of iterations ! " << maxNumOfIter << std::endl;
            break;
        }
        if (iter >= 2) {
            const double relEpsilon = epsilon / minExactObjVal;
            const double relGamma = gamma / exactObjVal;
            if (gamma < gammaTol) {
                #ifndef NDEBUG
                std::clog << "gamma criterion: " << gamma<< std::endl;
                #endif
                break;
            }

            if (epsilon < epsilonTol) {
              #ifndef NDEBUG
                std::clog << "epsilon criterion: " << epsilon<< std::endl;
                #endif
             
                break;
            }

            if (relGamma < relGammaTol) {
                 #ifndef NDEBUG
                std::clog << "relGamma criterion: " << relGammaTol << std::endl;
                #endif
             
                break;
            }

            if (relEpsilon < relEpsilonTol) {
                 #ifndef NDEBUG
                std::clog << "relEpsilon criterion: " << relEpsilonTol<< std::endl;
                #endif             
                break;
            }
        }



        // adjust inner solver optimization tolerance
        innerSolverTol = std::min<Real > (innerSolverTol, epsilon);

        // if the annealing doesn't work well, lower the inner solver tolerance
        if (prevEpsilon < epsilon) {
            innerSolverTol *= 0.2;
        }
        innerSolver->SetTolerance(innerSolverTol * 0.5);


        innerSolver->Solve(w, gradient, loss, approxObjVal);
    }

    // AK : passing w_final to w, have to check on that !!
    //    double diffnorm = norm_frobenius(w-w_final);
    // printf("\n AK Note,  ||w - w_final||  : %f \n", diffnorm);    
    w = w_final;
     #ifndef NDEBUG
      std::clog << "Final Loss " << loss<< std::endl;
     #endif
             
    return (loss);

}

void BMRM::setConvergence(double gammaTol, double epsilonTol, double relEpsilonTol, double relGammaTol, int maxIter) {
    this -> relGammaTol = relGammaTol;
    this -> relEpsilonTol = relEpsilonTol;
    this -> gammaTol = gammaTol;
    this -> epsilonTol = epsilonTol;
    this -> maxNumOfIter = maxIter;
}



#endif
