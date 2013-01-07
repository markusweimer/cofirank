#include "adaptiveregularizationlosswrapper.hpp"

void cofi::AdaptiveRegularizationLossWrapper::ComputeLossGradient(WType& w, Real &loss, cofi::WType& grad){
   // why dont we instead multiply w *= this->weight 
    this->lossFunction.ComputeLossGradient(w, loss, grad);
    loss *= this->weight;
    grad *= this->weight;
}

void cofi::AdaptiveRegularizationLossWrapper::ComputeLossPartGradient(WType& w, Real &loss, cofi::WType& grad){
    this->lossFunction.ComputeLossPartGradient(w, loss, grad);
    // loss *= this->weight;
    // grad *= this->weight;
}
