#include "meansquarederror.hpp"
#include <cmath>

static const std::string MSE = "rmse";

std::vector<std::string> cofi::MSEEvaluator::names(void){
    std::vector<std::string> result;
    result.push_back(MSE);
    return result;
}

void cofi::MSEEvaluator::eval(cofi::UserIterator& iter, std::map<std::string, double>& results){
    Real error = 0.0;
    size_t counter = 0;
    while (iter.hasNext()) {
        iter.advance();
        ublas::matrix<Real>& Y = iter.getY();
        ublas::matrix<Real>  F = ublas::matrix<Real>(Y.size1(), Y.size2());
        iter.predict(F);
        
        assert(F.size1() == Y.size1());
        assert(F.size2() == Y.size2());
        
        
        for(size_t i=0; i<Y.size1(); ++i){
            Real prediction = F(i, 0);
            error   += pow(Y(i, 0) - prediction, 2);
            counter += 1;
        }
        
    }
    error = error / ((Real) counter);
    results[MSE] = std::sqrt(error);
}
