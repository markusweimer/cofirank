#include "ndcgevaluator.hpp"
#include "utils/configuration.hpp"
#include "cofi/useriterator.hpp"
#include "utils/ublastools.hpp"
#include "loss/ndcgdomainmodel.hpp"
#include "utils/utils.hpp"


cofi::NDCGEvaluator::NDCGEvaluator(void):bigKWarned(false){
    truncation = Configuration::getInstance().getInt("cofi.eval.ndcg.k");
    name = "NDCG@"+to_string(truncation);
}

std::vector<std::string> cofi::NDCGEvaluator::names() {
    std::vector<std::string> result;
    result.push_back(name);
    return result;
}

void cofi::NDCGEvaluator::eval(cofi::UserIterator& iter, std::map<std::string, double>& results) {
    
    double NDCG_SUM = 0.0;
    size_t nUser = 0;
    
    while (iter.hasNext()) {
        ++nUser;
        iter.advance();
        const ublas::matrix<Real>& Y = iter.getY();
        size_t k = truncation;
        if (truncation > Y.size1()){
            if(!bigKWarned){
                std::clog << "NDCGEvaluator: k is bigger than Y.size1(). Using Y.size1() as k."<<std::endl;
                bigKWarned = true;
            }
            k = Y.size1();
        }
        const ublas::vector<size_t> sp = cofi::ublastools::decreasingSort<ublas::matrix<double> >(Y);
        const Real perfectDCG = NDCGDomainModel::dcg(Y, sp, k);
        
        ublas::matrix<Real> f(Y.size1(), Y.size2());
        iter.predict(f);
        assert(f.size1() == Y.size1() && f.size2() == Y.size2());
        const ublas::vector<size_t> pp = cofi::ublastools::decreasingSort<ublas::matrix<double> >(f);
        const Real predictedDCG = NDCGDomainModel::dcg(Y, pp, k);
        
        NDCG_SUM += predictedDCG/perfectDCG;
    }
    const double avg_ndcg = NDCG_SUM / static_cast<double>(nUser);
    
    results[name] = avg_ndcg;
}
