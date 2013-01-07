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
 *                Alexandros Karatzoglou (alexis@ci.tuwien.ac.at)
 *
 * Created      : 10/10/2007
 *
 * Last Updated :
 */

#include "problem.hpp"
#include "utils/configuration.hpp"
#include "io/svmlightreader.hpp"
#include "core/cofiexception.hpp"
#include "utils/ublastools.hpp"
#include "cofi/useriterator.hpp"
#include "io/io.hpp"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>


cofi::Problem::~Problem(void) {
    if (trainD) delete trainD;
    if (testD) delete testD;
    if (U) delete U;
    if (M) delete M;
    if (A) delete A;
    if (S) delete S;
    if (bestM) delete bestM;
}


cofi::Problem::Problem(void) :
useMovieOffset(false), useUserOffset(false), evalMode(WEAK), trainD(NULL), testD(NULL),
S(NULL), U(NULL), M(NULL), A(NULL), bestM(NULL) {

    // Configuration parsing
    Configuration& conf = Configuration::getInstance();
    const std::string mode = conf.getString("cofibmrm.evaluation");
    if (mode == "STRONG") {
        this->evalMode = STRONG;
        std::clog << "Evaluation Mode: STRONG" << std::endl;
    } else if (mode == "WEAK") {
        this->evalMode = WEAK;
        std::clog << "Evaluation Mode: WEAK" << std::endl;
    } else {
        throw CoFiException("Evaluation mode needs to be either STRONG or WEAK");
    }
    this->useGraphKernel = conf.getIntAsBool("cofi.useGraphKernel");
    this->useMovieOffset = conf.getIntAsBool("cofi.useMovieOffset");
    this->useUserOffset = conf.getIntAsBool("cofi.useUserOffset");
    this->useAdaptiveRegularization = conf.getIntAsBool("cofi.useAdaptiveRegularization");
    this->dimW = conf.getInt("cofi.dimW");

    if (usingMovieOffset()) {
        std::clog << "Enabling movie offset" << std::endl;
        this->dimW += 1;
    }
    if (usingUserOffset()) {
        std::clog << "Enabling user offset" << std::endl;
        this->dimW += 1;
    }
    this->nMovies = 0;
    this->setupD(); // Also set nMovies to something sensible
    assert(this-nMovies > 0);
    const size_t nUsers = trainD->size1();

    // Setup M
    this->M = new cofi::MType(nMovies, dimW);
    cofi::ublastools::random<cofi::MType > (*M);
    if (usingUserOffset()) {
        setUserOffsetColumnInMToOne();
    }

    // Setup U
    this->U = new cofi::UType(nUsers, dimW);
    cofi::ublastools::random<cofi::UType > (*U);
    if (usingMovieOffset()) {
        setMovieOffsetColumnInUToOne();
    }



    if (usingAdaptiveRegularization()) {
        std::clog << "Enabling Adaptive Regularization" << std::endl;
        const Real exponent = conf.getDouble("cofi.adaptiveRegularization.uExponent");
        computeWeights(exponent);
    }

    // Setup S and A if we are using graph kernels
    if (usingGraphKernel()) {
        std::clog << "Enabling Graph Kernel" << std::endl;
        setupS();
        A = new cofi::MType(M->size1(), M->size2());
        cofi::ublastools::random<cofi::UType > (*A);
    }
    assert(trainD->size1() == testD->size1());
    assert(trainD->size2() == testD->size2());
    assert(U->size2() == M->size2());
    assert(U->size1() == trainD->size1());
    assert(M->size1() == trainD->size2());
}


cofi::UserIterator cofi::Problem::getTrainIterator(void) {
    return UserIterator(*this, cofi::UserIterator::TRAINING);
}


cofi::UserIterator cofi::Problem::getTestIterator(void) {
    return UserIterator(*this, cofi::UserIterator::TESTING);
}


void cofi::Problem::setupD(void) {
    Configuration& conf = Configuration::getInstance();
    const std::string testFileName = conf.getString("cofibmrm.DtestFile");
    const std::string trainFileName = conf.getString("cofibmrm.DtrainFile");

    std::pair<size_t, size_t> trainDims = cofi::io::getDimensions<cofi::DType > (trainFileName);
    std::pair<size_t, size_t> testDims = cofi::io::getDimensions<cofi::DType > (testFileName);

    const size_t rows = trainDims.first;
    assert(rows == testDims.first);
    this->nMovies = std::max(testDims.second, trainDims.second);

    std::clog << "Problem:: we have " << rows << " rows and " << nMovies << " columns in D" << std::endl;

    if (getEvaluationMode() == STRONG) {
        const std::string testStrongFileName = conf.getString("cofibmrm.DtestStrongFile");
        const std::string trainStrongFileName = conf.getString("cofibmrm.DtrainStrongFile");
        std::pair<size_t, size_t> trainStrongDims = cofi::io::getDimensions<cofi::DType > (trainStrongFileName);
        std::pair<size_t, size_t> testStrongDims = cofi::io::getDimensions<cofi::DType > (testStrongFileName);
        assert(trainStrongDims.first == testStrongDims.first);
        size_t n = std::max<size_t > (trainStrongDims.second, testStrongDims.second);
        this->nMovies = std::max<size_t > (this->nMovies, n);
    }

    std::clog << "Problem: Reading train data from " << trainFileName << std::endl;
    this->trainD = new cofi::DType(rows, nMovies);
    cofi::io::loadMatrixWithOutResize(*(this->trainD), trainFileName);

    std::clog << "Problem: Reading test data from " << testFileName << std::endl;
    this->testD = new cofi::DType(rows, nMovies);
    cofi::io::loadMatrixWithOutResize(*(this->testD), testFileName);
}


void cofi::Problem::switchToStrongGeneralization(void) {
    assert(getEvaluationMode() == STRONG);
    //Clean up data from Weak run
    if (trainD) delete trainD;
    if (testD) delete testD;
    if (U) delete U;

    Configuration& conf = Configuration::getInstance();
    const std::string testFileName = conf.getString("cofibmrm.DtestStrongFile");
    const std::string trainFileName = conf.getString("cofibmrm.DtrainStrongFile");


    std::pair<size_t, size_t> trainDims = cofi::io::getDimensions<cofi::DType > (trainFileName);
    std::pair<size_t, size_t> testDims = cofi::io::getDimensions<cofi::DType > (testFileName);
    const size_t rows = trainDims.first;
    assert(rows == testDims.second);

    std::clog << "Problem: Reading Strong Generalization train data from the SVMLIGHT file " << trainFileName << std::endl;
    this->trainD = new cofi::DType(rows, this->nMovies);
    cofi::io::loadMatrixWithOutResize(*(this->trainD), trainFileName);

    std::clog << "Problem: Reading Strong Generalization test data from the SVMLIGHT file " << testFileName << std::endl;
    this->testD = new cofi::DType(rows, this->nMovies);
    cofi::io::loadMatrixWithOutResize(*(this->testD), testFileName);


    const size_t nUsers = trainD->size1();
    this->U = new cofi::UType(nUsers, dimW);
    cofi::ublastools::random<cofi::UType > (*U);

    // Setup S and A if we are using graph kernels
    if (usingGraphKernel()) {
        std::clog << "Enabling Graph Kernel" << std::endl;
        setupS();
        A = new cofi::MType(nMovies, dimW);
        cofi::ublastools::random<cofi::UType > (*A);
    }
}


void cofi::Problem::setUserOffsetColumnInMToOne(void) {
    for (size_t row = 0; row < (*M).size1(); ++row) {
        (*M)(row, cofi::USER_OFFSET_COLUMN) = 1;
    }
}


void cofi::Problem::setUserOffsetColumnInMToZero(void) {
    for (size_t row = 0; row < (*M).size1(); ++row) {
        (*M)(row, cofi::USER_OFFSET_COLUMN) = 0;
    }
}


void cofi::Problem::setMovieOffsetColumnInUToOne(void) {
    for (size_t row = 0; row < (*U).size1(); ++row) {
        (*U)(row, cofi::MOVIE_OFFSET_COLUMN) = 1;
    }
}


void cofi::Problem::computeWeights(const Real exponent) {
    this->weightsU = ublas::vector<Real > (getNumberOfUsers());
    this->weightsU.clear(); // make it empty

    // Count the seen movies.
    typedef cofi::DType::iterator1 rowIterator;
    typedef cofi::DType::iterator2 columnIterator;

    Real maxCount = 0;

    for (rowIterator i1 = trainD->begin1(); i1 != trainD->end1(); ++i1) {
        for (columnIterator i2 = i1.begin(); i2 != i1.end(); ++i2) {
            const size_t user = i2.index1();
            weightsU[user] += 1;
            maxCount = std::max<Real > (maxCount, weightsU[user]);
        }
    }

    // Normalize the counts
    weightsU /= maxCount;

    // Exponentiate the entries with the factor
    for (size_t i = 0; i < weightsU.size(); ++i) {
        weightsU[i] = pow(weightsU[i], exponent);
    }

}


void cofi::Problem::setupS(void) {
    S = new SType(testD->size1(), testD->size2());
    S->clear();
    for (DType::iterator1 row = trainD->begin1(); row != trainD->end1(); ++row) {
        Real count = 0;
        for (DType::iterator2 col = row.begin(); col != row.end(); ++col) {
            count += 1.0;
        }
        for (DType::iterator2 col = row.begin(); col != row.end(); ++col) {
            (*S)(col.index1(), col.index2()) = 1.0 / count;
        }
    }
}


void cofi::Problem::save(std::string prefix) {
    std::clog << "cofi::Problem::save(): Save is disabled. " << std::endl;
    //
    //    Configuration& conf = Configuration::getInstance();
    //    const std::string& outFolder = conf.getString("cofi.outfolder");
    //    // Store U, M
    //    std::clog << "Storing trained F, U and M" << std::endl;
    //    if (conf.getIntAsBool("cofi.storeU")) {
    //        cofi::io::storeMatrix<cofi::UType > (*U, outFolder + "U_" + prefix + ".lsvm");
    //        if (usingGraphKernel()) {
    //            cofi::io::storeMatrix<cofi::SType > (*S, outFolder + "S_" + prefix + ".lsvm");
    //            cofi::io::storeMatrix<ublas::matrix<Real> >(*A, outFolder + "A_" + prefix + ".lsvm");
    //        }
    //    }
    //    if (conf.getIntAsBool("cofi.storeM")) {
    //        cofi::io::storeMatrix<cofi::MType > (*M, outFolder + "M_" + prefix + ".lsvm");
    //        if (bestM)
    //            cofi::io::storeMatrix<cofi::MType > (*bestM, outFolder + "M_best_" + prefix + ".lsvm");
    //    }
    //    if (conf.getIntAsBool("cofi.storeF")) {
    //        ublas::matrix<Real> F = prod(*U, trans(*M));
    //        cofi::io::storeMatrix<ublas::matrix<Real> >(F, outFolder + "F_" + prefix + ".lsvm");
    //    }
}

