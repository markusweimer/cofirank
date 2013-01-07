#include "graphkernellosswrapper.hpp"
#include "core/types.hpp"
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include "core/cofiexception.hpp"
#include "cofi/useriterator.hpp"

cofi::GraphKernelLossWrapper::GraphKernelLossWrapper(Problem& p):p(p){}


void cofi::GraphKernelLossWrapper::ComputeLossGradient(WType& w, Real& loss, cofi::WType& grad){

    assert(w.size1() == grad.size1());
    assert(w.size2() == grad.size2());
    const size_t u = p.getU().size1();
    const size_t m = p.getNumberOfItems();
    const size_t d = p.getDimW();
    //Clear gradient
    grad.clear();
    // Copy the given W back into the problem
    p.getU() = ublas::subrange(w, 0, u, 0, d);
    p.getA() = ublas::subrange(w, u, u+m, 0, d);
    
    //Initialize iterator over nonzero elements of D
    itr1 mit1 = p.getS().begin1();
    
    // Compute the gradient for each user and store it in grad's first nUsers rows
    cofi::UserIterator userIter = p.getTrainIterator();
    size_t counter = 0;
    loss=0.0;

    while(userIter.hasNext()){
        userIter.advance();
        cofi::WType& thisW = userIter.getW(); // We operate on the plain W
        cofi::WType thisGradient(thisW.size1(), thisW.size2());

        Real thisLoss = 0;
        if(p.usingAdaptiveRegularization()){
            userIter.getWeightedLoss().ComputeLossGradient(thisW, thisLoss, thisGradient); 
        }
        else{
            userIter.getLoss().ComputeLossGradient(thisW, thisLoss, thisGradient); 
        }
        
        ublas::row(grad, counter) = ublas::column(thisGradient, 0);
        
        itr2 mit2 = mit1.begin();
     
       // Compute column component of S'grad and store it in remaining rows of grad 
       while(mit2 != mit1.end()){
           ublas::row(grad, mit2.index2() + u) += p.getS()(counter, mit2.index2()) * column(thisGradient,0);         
            ++mit2;
        }
        ++mit1; 
         
        loss += thisLoss;
        counter += 1;
    }

    assert(counter == u);
       
   // Compute S'grad and store it in the remaining rows of grad
   // subrange(grad,u,u+m,0,d) = prod(trans(p.getS()), subrange(grad, 0, u, 0, d));
   // subrange(grad,u,u+m,0,d) *= 0;
}
