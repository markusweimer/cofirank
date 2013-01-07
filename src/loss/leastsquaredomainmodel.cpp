#include "leastsquaredomainmodel.hpp"
#include <cassert>


LeastSquareDomainModel::LeastSquareDomainModel(const ublas::matrix<Real>& X, const ublas::matrix<Real>& Y) : X(X), Y(Y) {
    assert(X.size1() == Y.size1());
}


void LeastSquareDomainModel::ComputeLossGradient(cofi::WType& w, Real &loss, cofi::WType& grad) {
    assert(w.size1() == X.size2());
    assert(w.size1() == grad.size1());
    assert(w.size2() == grad.size2());
    // Gradient with respect to f
    ublas::matrix<Real> g(Y.size1(), Y.size2());
    ComputeLossPartGradient(w, loss, g);
    assert(loss >= 0);

    // Make gradient with respect to w
    grad = prod(trans(X), g);

}


void LeastSquareDomainModel::ComputeLossPartGradient(cofi::WType& w, Real &loss, cofi::WType& grad) {
    assert(Y.size1() == grad.size1());
    assert(Y.size2() == grad.size2());
    const ublas::matrix<Real> f = prod(X, w);
    assert(f.size1() == Y.size1());
    assert(f.size2() == Y.size2());

    loss = 0;
    for (size_t i = 0; i < Y.size1(); i++) {
        const Real value = f(i, 0) - Y(i, 0);
        const Real gradient_i = 2 * value;
        const Real loss_i = value*value;
        grad(i, 0) = gradient_i;
        loss += loss_i;
    }
}


LeastSquareDomainModel::~LeastSquareDomainModel(void) {
}
