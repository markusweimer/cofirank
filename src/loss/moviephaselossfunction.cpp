/* Copyright (c) 2006, National ICT Australia
 * All rights reserved.
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Authors      : Alexandros Karatzoglou (alexis@ci.tuwien.ac.at)
 *
 * Created      : 21/06/2007
 *
 * Last Updated :
 */



#ifndef _DOMAINWRAPPER_CPP_
#define _DOMAINWRAPPER_CPP_

#include <cassert>
#include "moviephaselossfunction.hpp"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include "cofi/useriterator.hpp"
#include "core/cofiexception.hpp"


cofi::MoviePhaseLossFunction::MoviePhaseLossFunction(cofi::Problem& p) : p(p) {
    nUser = p.getU().size1(); // number of users
    nMovies = p.getM().size1(); // number of movies
}


void cofi::MoviePhaseLossFunction::ComputeLossGradient(cofi::WType& w, Real &loss, cofi::WType& grad) {
    assert(w.size1() == grad.size1());
    assert(w.size2() == grad.size2());

    // We need to clear the gradient since we now add the result from each column
    // computation
    grad.clear();

    //Initialize iterator over nonzero elements of D
    itr1 mit1 = p.getTrainD().begin1();

    // The (partial derivative of L with respect to f)'
    //compressed_matrix<double> Atmp(nMovies, nUser, p.getTrainD().nnz());

    // We should get M as w
    assert(&(p.getM()) == &w);

    // Offset management:
    // If we have an offset, we need to make sure that bmrm does not optimize it
    // away. We do two things to assure this:
    //
    // (1) We set the column of the bias to zero before calling BMRM and set it
    //     to 1 after optimization.
    // (2) In the loss computation, we set it to 1 for the loss and gradient
    //     computation.
    if (p.usingUserOffset()) {
        p.setUserOffsetColumnInMToOne();
    }

    // optimization loop
    Real Loss = 0.0; // per dataset loss
    cofi::UserIterator userIter = p.getTrainIterator();
    while (userIter.hasNext()) {

        userIter.advance();
        cofi::WType& W = userIter.getW();
        const int rowInU = userIter.getRowInU();

        CofiLossFunction& model = userIter.getLoss();
        const size_t seenMovies = userIter.getX().size1();
        // Per user gradient
        ublas::matrix<Real> atmp = ublas::matrix<Real > (userIter.getY().size1(), userIter.getY().size2());

        Real tmpLoss = 0.0;
        model.ComputeLossPartGradient(W, tmpLoss, atmp);

        Loss = Loss + tmpLoss;

        //copy result into Atmp matrix
        itr2 mit2 = mit1.begin();

        //     Setup matrix a to contain the gradient for each user as one column
        //        for (size_t row = 0; row < seenMovies ; ++row) {
        //            Atmp(mit2.index2(), (userID-1)) = atmp(row, 0);
        //            ++mit2;
        //        }// all rows
        //        ++mit1;

        // Decompose the matrix multiplication (\partial_F L)' * U into operations
        // over each gradient (seems much faster!)
        for (size_t row_i = 0; row_i < seenMovies; ++row_i) {
            ublas::row(grad, mit2.index2()) += atmp(row_i, 0) * ublas::row(p.getU(), rowInU);
            ++mit2;
        }
        ++mit1;
    }
    loss = Loss;

    if (p.usingUserOffset()) {
        p.setUserOffsetColumnInMToZero();

        for (size_t row = 0; row < grad.size1(); ++row) {
            grad(row, cofi::USER_OFFSET_COLUMN) = 0;

            // w here is M. Thus, the corresponding column of w should be 0 now.
            assert(w(row, cofi::USER_OFFSET_COLUMN) == 0);
        }


    }
    // This comoutes (\partial_M L)' * U
    //grad = prod(Atmp, p.getU());

}
#endif
