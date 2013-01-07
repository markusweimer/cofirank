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
 * Created      : 21/05/2007
 *
 * Last Updated :
 */
#include "usertrainer.hpp"
#include <cassert>
#include "loss/graphkernellosswrapper.hpp"
#include "cofi/useriterator.hpp"
#include "solver.hpp"
#include "loss/userloss.hpp"

Real cofi::UserTrainer::run(cofi::Problem& p, size_t t, Real lambda) {
#ifndef NDEBUG
    std::clog << "cofi::UserTrainer::run: lambda=" << lambda << std::endl;
#endif

    assert(lambda > 0);
    if (p.usingGraphKernel()) {
        const size_t u = p.getU().size1();
        const size_t m = p.getNumberOfItems();
        const size_t d = p.getDimW();
        // Create W
        cofi::WType W = cofi::WType(u + m, d);
        // Copy U and A into it
        ublas::subrange(W, 0, u, 0, d) = p.getU();
        ublas::subrange(W, u, u + m, 0, d) = p.getA();

        cofi::GraphKernelLossWrapper lossFunction(p);
        cofi::Solver solver;
        const Real loss = solver.optimize(W, lossFunction, lambda, t);

        // Copy W back into A and U
        p.getU() = ublas::subrange(W, 0, u, 0, d);
        p.getA() = ublas::subrange(W, u, u + m, 0, d);
        return loss;
    }else {
        cofi::UserIterator iter = p.getTrainIterator();
        Real lossSum = 0.0;
        cofi::Solver solver;
        while (iter.hasNext()) {
            iter.advance();
            LossFunction* loss = NULL;
            if (p.usingAdaptiveRegularization()) {
                loss = new UserLoss(iter.getWeightedLoss(), p.usingMovieOffset());
            } else {
                loss = new UserLoss(iter.getLoss(), p.usingMovieOffset());
            }
            lossSum += solver.optimize(iter.getW(), *loss, lambda, t);
#ifndef NDEBUG
	    std::clog << "User # : " << iter.getRowInU() << "  Cumulative Loss : " <<  lossSum << std::endl;
#endif
            iter.updateW();
            delete loss;
        }// while all users
        if (p.usingMovieOffset()) {
            p.setMovieOffsetColumnInUToOne();
        }
        return lossSum;
    }// if not using graph kernel

}
