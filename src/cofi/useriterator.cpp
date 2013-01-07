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
 * Created      : 27/05/2007
 *
 * Last Updated :
 */

#include "useriterator.hpp"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

#include <boost/numeric/ublas/matrix_sparse.hpp>
#include "loss/lossfunctionfactory.hpp"
#include "core/cofiexception.hpp"
#include "loss/adaptiveregularizationlosswrapper.hpp"

namespace ublas = boost::numeric::ublas;

cofi::UserIterator::UserIterator(cofi::Problem& p, Phase phase):
p(p), phase(phase),
        X(NULL), Y(NULL), W(NULL), loss(NULL), weightedLoss(NULL) {
    
    if(phase == TRAINING){
        dRows = p.getTrainD().begin1();
    }
    else if (phase == TESTING){
        dRows = p.getTestD().begin1();
    }
    else{throw CoFiException("UserIterator::Phase should be either TRAINING or TESTING");}
    if(p.usingGraphKernel()){
        SA = prod(p.getS(), p.getA());
        assert(SA.size1() == p.getU().size1());
        assert(SA.size2() == p.getU().size2());
    }
    
    nextRow = 0;
}


void cofi::UserIterator::updateW() {
    row(p.getU(), getRowInU()) = column(*(this->W), 0);
}


cofi::UserIterator::~UserIterator(void) {
    clear();
}


bool cofi::UserIterator::hasNext(void) {
    if(phase == TRAINING){
        return dRows != p.getTrainD().end1();
    }
    else{
        assert(phase == TESTING);
        return dRows != p.getTestD().end1();
    }
    
}

void cofi::UserIterator::advance(void) {
    
    
    const size_t userID = nextRow;
    this->nextRow += 1;
    
    assert(dRows.index1() == userID);
    
    
    // Clear X,Y,W,loss, countM
    clear();
    
    // Setup
    // w = U[userID, *]
    
    this->W = new cofi::WType(p.getDimW(), 1);
    column(*(this->W), 0) = row(p.getU(), userID);
    if(p.usingGraphKernel()){
        ublas::column(*(this->W), 0) += ublas::row(SA, userID);
    }
    
    // -------------------------------------------------------------------------
    // Setup X and Y as a dense problem.
    //
    // This is done by extracting only those movies which the user has actually
    // seen. Those rows of M will form X.
    //
    // The entries of D for these movies will form Y.
    // const size_t rows = (*nonzeros)[userID].size();
    size_t rows = 0;
    for(colIteratorType cIter = dRows.begin(); cIter != dRows.end();++cIter){
        ++rows;
    }
    
    this->X = new ublas::matrix<Real>(rows, p.getDimW());
    this->Y = new ublas::matrix<Real>(rows, 1);
    
    
    colIteratorType columnIter = dRows.begin();
    for (size_t row = 0; row < rows ; ++row) {
        const size_t movieID = columnIter.index2();
        for (size_t col = 0; col < p.getDimW(); ++col) {
            (*X)(row, col) = p.getM()(movieID, col);
        }
        (*Y)(row, 0) = *columnIter;
        ++columnIter;
    }
    assert(columnIter == dRows.end());
    
    // Switch the iterator to the next line.
    ++dRows;
}


CofiLossFunction& cofi::UserIterator::getLoss(void){
    this->loss = LossFunctionFactory::getInstance().get(*X, *Y);
    return *(this->loss);
}

CofiLossFunction& cofi::UserIterator::getWeightedLoss(void){
    assert(p.usingAdaptiveRegularization());
    const Real weight = p.getWeightForU(getRowInU());
    this->weightedLoss = new AdaptiveRegularizationLossWrapper(weight, getLoss());
    return *(this->weightedLoss);
}


cofi::WType& cofi::UserIterator::getW(void){
    return *(this->W);
}


void cofi::UserIterator::predict(ublas::matrix<Real>& F){
    getLoss().predict(getW(), getX(), F);
}


void cofi::UserIterator::clear(void) {
    if(this->X)            delete this->X;
    if(this->Y)            delete this->Y;
    if(this->loss)         delete this->loss;
    if(this->weightedLoss) delete this->weightedLoss;
    if(this->W)            delete this->W;    
}
