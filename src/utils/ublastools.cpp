#include "ublastools.hpp"

/**
 * Computes the inverse permutation of pi
 */
boost::numeric::ublas::vector<size_t> cofi::ublastools::invert_permutation(const boost::numeric::ublas::vector<size_t>& pi) {
    boost::numeric::ublas::vector<size_t> result(pi.size());
    for(size_t i = 0; i<pi.size(); ++i){
        result[pi[i]] = i;
    }
    return result;
}


Real cofi::ublastools::mean(const ublas::matrix<Real>& M){
    return norm_1(M)/(M.size1()*M.size2());
}

Real cofi::ublastools::mean(const ublas::compressed_matrix<Real>& M){
    return norm_1(M)/M.nnz();
}
