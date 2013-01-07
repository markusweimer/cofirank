/* The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Authors      : Markus Weimer       (cofirank@weimo.de)
 *
 * Last Updated :
 */
#include "ndcgdomainmodel.hpp"
#include <cmath>
#include <cassert>
#include "lap.hpp"
#include "utils/configuration.hpp"
#include "core/cofiexception.hpp"
#include "utils/ublastools.hpp"
#include "utils/utils.hpp"


NDCGDomainModel::NDCGDomainModel(const ublas::matrix<Real>& X, const ublas::matrix<Real>& Y) :
X(X), Y(Y) {

    Configuration& conf = Configuration::getInstance();
    this->trainK = conf.getInt("loss.ndcg.trainK");
    if (this->trainK == 0) {
        std::clog << "NDCGDomainModel:: Training NDCG@infinity" << std::endl;
        this->trainK = X.size1();
    }
    const double c_exponent = conf.getDouble("loss.ndcg.c_exponent");

    // Check the configuration for consistency.

    if (this->trainK > X.size1()) {
        std::string k = to_string(trainK);
        throw cofi::InvalidParameterException("NDCGDomainModel::NDCG Truncation of " + k + " is bigger than the number of rows in X and Y.");
    }

    // Compute the sort and the DCG of that sort
    ublas::vector<size_t> decreasingSort = cofi::ublastools::decreasingSort<ublas::matrix<double> >(Y);
    perfectDCG = dcg(Y, decreasingSort, trainK);
    if (perfectDCG > std::numeric_limits<Real>::max()) {
        throw cofi::NumericException("NDCG computation overflow when computing perfectDCG.");
    }

    // Compute c
    c = ublas::vector<Real > (Y.size1());
    for (size_t i = 0; i < c.size(); ++i) {
        c[i] = pow((i + 1.0), c_exponent);
    }
}


void NDCGDomainModel::ComputeLossGradient(cofi::WType& w, Real &loss, cofi::WType& grad) {
    assert(w.size1() == grad.size1());
    assert(w.size2() == grad.size2());
    // Gradient with respect to f
    ublas::matrix<Real> g(Y.size1(), 1);
    ComputeLossPartGradient(w, loss, g);

    // Make gradient with respect to w
    grad = prod(trans(X), g);
}


void NDCGDomainModel::ComputeLossPartGradient(cofi::WType& w, Real &loss, cofi::WType& grad) {
    const ublas::matrix<Real> f = prod(X, w);
    ublas::vector<int> pi(Y.size1());

    find_permutation(f, pi);
    ublas::vector<size_t> pii = cofi::ublastools::invert_permutation(pi); // Inverse permutation

    Real scalarProd = 0; // <c, (f_pi - f)>
    for (size_t i = 0; i < Y.size1(); i++) {
        scalarProd += c[i] * (f(pi[i], 0) - f(i, 0));
        grad(i, 0) = c[pii[i]] - c[i];
    }
    loss = delta(pi) + scalarProd;
    //assert(grad.size1() == f.size1());
    //assert(grad.size2() == 1);
}


/* compute the dcg of Y and store in dy */
Real NDCGDomainModel::dcg(const ublas::matrix<Real>& y, const ublas::vector<size_t>& pi, const size_t k) {
    if (y.size1() < k) {
        throw cofi::InvalidParameterException("k is bigger than the length of Y.");
    }

    if (pi.size() < k) {
        throw cofi::InvalidParameterException("The given permutation vector pi is too small");
    }

    if (pi.size() != y.size1()) {
        throw cofi::InvalidParameterException("The given permutation vector pi has a different size than the vector Y.");
    }

    Real d = 0.0;

    for (size_t i = 0; i < k; i++) {
        d += (pow(2.0, y(pi[i], 0)) - 1.0) / log2(i + 2.0);
    }

    assert(d > 0.0);

    return d;
}


/* compute the loss and store in value */
Real NDCGDomainModel::delta(const ublas::vector<size_t>& pi) {
    const Real theDCG = dcg(Y, pi, trainK);
    const Real nDCG = theDCG / perfectDCG;
    const Real result = 1.0 - nDCG;
    assert(result >= 0.0 && result <= 1.0);
    return result;
}


/* compute the permutation by linear assignment and store in pi */
void NDCGDomainModel::find_permutation(const ublas::matrix<Real> &f, ublas::vector<int>& pi) {
    /* setting up C_ij */
    Real **C = new Real*[Y.size1()];

    for (size_t i = 0; i < Y.size1(); i++) {
        C[i] = new Real[Y.size1()];
        for (size_t j = 0; j < Y.size1(); j++) {
            if (i < trainK) {
                C[i][j] = ((pow(2, Y(j, 0)) - 1) / log2(i + 2)) / perfectDCG - c[i] * f(j, 0);
            } else {
                C[i][j] = -c[i] * f(j, 0);
            }
        }
    }

    ublas::vector<int> row(Y.size1());

    lap(Y.size1(), C, &pi(0), &row(0));

    for (size_t i = 0; i < Y.size1(); i++) {
        delete C[i];
    }
    delete C;
}
