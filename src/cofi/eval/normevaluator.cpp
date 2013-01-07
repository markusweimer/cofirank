#include "normevaluator.hpp"

const static std::string NU="norm(U)";
const static std::string NM="norm(M)";
const static std::string NA="norm(A)";



cofi::NormEvaluator::NormEvaluator(void){
}

cofi::NormEvaluator::~NormEvaluator(void){
}

std::vector<std::string> cofi::NormEvaluator::names(){
    std::vector<std::string> result;
    result.push_back(NU);
    result.push_back(NM);
    result.push_back(NA);
    return result;
}

void cofi::NormEvaluator::eval(cofi::Problem& p, std::map<std::string, double>& results){
    results[NU] = norm_frobenius(p.getU());
    results[NM] = norm_frobenius(p.getM());
    if(p.usingGraphKernel()){
        results[NA] = norm_frobenius(p.getA());
    }
    else{
        results[NA] = -1.0;
    }
}
