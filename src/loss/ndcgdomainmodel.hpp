#ifndef _NDCGDOMAINMODEL_HPP_
#define _NDCGDOMAINMODEL_HPP_

#include "cofilossfunction.hpp"

/**
 * NDCG Loss.
 *
 */
class NDCGDomainModel : public CofiLossFunction {
    
public:
    /**
     * @param X the samples to learn from
     * @param Y the labels for the given samples
     * @param n the truncation cutoff, the n in NDCG@n
     */
    NDCGDomainModel(const ublas::matrix<Real>& X, const ublas::matrix<Real>& Y);
    ~NDCGDomainModel(){};
    
    
    void ComputeLossGradient(cofi::WType& w, Real &loss, cofi::WType& grad);
    void ComputeLossPartGradient(cofi::WType& w, Real &loss, cofi::WType& grad);
    
    /**
     * Computes the DCG of the given matrix when the matrix is permutated using the given pi.
     *
     * @param Y the input matrix. Only the first column will be used. Thus, the matrix is treated as a vector.
     * @param pi the permutation vector. pi[i] is the index of element i in the permutated matrix.
     */
    static Real dcg(const ublas::matrix<Real>& Y, const ublas::vector<size_t>& pi, const size_t k);
    
    /**
     * Computes the delta in NDCG between the optimal solution and the given permutation.
     *
     * This version uses the cached vectors a and b.
     *
     */
    Real delta(const ublas::vector<size_t>& pi);
    
private:
    
    void find_permutation(const ublas::matrix<Real> &f, ublas::vector<int>& pi);
    
    // Attributes
    const ublas::matrix<Real>& X;
    const ublas::matrix<Real>& Y;
    size_t trainK;
    Real perfectDCG;
    ublas::vector<Real> c;
};

#endif
