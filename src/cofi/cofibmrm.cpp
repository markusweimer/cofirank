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
 * Created      : 28/05/2007
 *
 * Last Updated :
 */
#include "cofibmrm.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "utils/configuration.hpp"
#include "cofi/eval/csvfileevaluator.hpp"
#include "cofi/eval/cofievaluator.hpp"
#include "io/io.hpp"
#include "utils/utils.hpp"
#include "utils/ublastools.hpp"

namespace cofi {

    /**
     * A simple evaluator for our evaluation framework that tracks the objective
     * values.
     *
     * This class should never ever be used outside the CofiBMRM file. It is 
     * based on public attributes and such.
     *
     */
    class ObjectiveEvaluator : public DataIndependentEvaluator {
    public:


        ObjectiveEvaluator(void) : ofVal(0), uLambda(0), mLambda(0), uLoss(0), mLoss(0), uNorm(0), mNorm(0) {
        };


        ~ObjectiveEvaluator(void) {
        };


        /**
         * @return the name of the evaluation measure
         */
        std::vector<std::string> names(void) {
            std::vector<std::string> result;
            result.push_back("objectiveFunctionValue");
            result.push_back("userLoss");
            result.push_back("userLambda");
            result.push_back("userNorm");
            result.push_back("movieLoss");
            result.push_back("movieLambda");
            result.push_back("movieNorm");
            return result;
        }


        /**
         * This just writes whatever is currently in the public(!) attributes to 
         * the result map.
         *
         * @param p the problem which holds the data to evaluate on.
         * @return the evaluation result.
         */
        void eval(cofi::Problem& p, std::map<std::string, double>& results) {
            results["objectiveFunctionValue"] = ofVal;
            results["userLoss"] = uLoss;
            results["userLambda"] = uLambda;
            results["userNorm"] = uNorm;
            results["movieLoss"] = mLoss;
            results["movieLambda"] = mLambda;
            results["movieNorm"] = mNorm;
        }

        Real ofVal;
        Real uLambda;
        Real mLambda;
        Real uLoss;
        Real mLoss;
        Real uNorm;
        Real mNorm;
    };

}


cofi::COFIBMRM::COFIBMRM(Problem& p) : p(p) {
    Configuration& conf = Configuration::getInstance();
    outFolder = conf.getString("cofi.outfolder");
    minIterations = conf.getInt("cofi.minIterations");
    maxIterations = conf.getInt("cofi.maxIterations");
    allowedDivergence = conf.getDouble("cofi.allowedDivergence");
    movieLambda = conf.getDouble("cofi.moviephase.lambda");
    userLambda = conf.getDouble("cofi.userphase.lambda");
    assert(movieLambda > 0.0);
    assert(userLambda > 0.0);
}


bool cofi::COFIBMRM::isConverged() {
    if (iteration < 1) return false;
    const Real currentValue = objectiveFunctionValues[iteration - 1];
    const Real bestValue = *(std::min_element(objectiveFunctionValues.begin(), objectiveFunctionValues.end()));
    const Real divergence = currentValue / bestValue - 1.0;
#ifndef NDEBUG
    std::clog << "CoFi Converge Info:" << std::endl;
    std::clog << "current: " << currentValue << ", best: " << bestValue << ", divergence: " << divergence << std::endl;
#endif

    if (iteration <= minIterations) return false;
    if (iteration > maxIterations) return true;
    return divergence > this->allowedDivergence;

}


void cofi::COFIBMRM::train(void) {
    UserTrainer userPhase;
    MovieTrainer moviePhase;

    std::ofstream out((outFolder + "result.csv").c_str());
    CSVFileEvaluator eval(out);
    eval.registerConfiguredEvaluators();
    ObjectiveEvaluator* ofEval = new ObjectiveEvaluator();
    eval.registerEvaluator(ofEval);
    eval.eval(p);

    Real minMLoss = 1e9;
    Real uLoss = 1e9;
    Real mLoss = 1e9;
    Real mNorm = 1e9;
    Real uNorm = 1e9;



    for (this->iteration = 0; not isConverged(); ++iteration) {

        //**********************************************************************
        // User Phase
        //        const Real objectiveBeforeUserPhase = mLoss + movieLambda * mNorm + userLambda * uNorm;
        std::clog << "COFIBMRM: User Phase started in iteration " << iteration << std::endl;
        uLoss = userPhase.run(p, this->iteration, this->userLambda) / p.getNumberOfUsers();
        uNorm = norm_frobenius(p.getU());

        //        const Real objectiveAfterUserPhase = uLoss + movieLambda * mNorm + userLambda * uNorm;

        assert(uLoss > 0);
        assert(uNorm > 0);
        this->userLosses.push_back(uLoss);
        this->userNorms.push_back(uNorm);
        this->userLambdas.push_back(userLambda);

        std::clog << "COFIBMRM: UserPhase finished " << std::endl;
        // End of User Phase
        //**********************************************************************



        //**********************************************************************
        // Movie Phase
        //        const Real objectiveBeforeMoviePhase = uLoss + movieLambda * mNorm + userLambda * uNorm;
        std::clog << "COFIBMRM: Movie Phase started in iteration " << iteration << std::endl;
        mLoss = moviePhase.run(p, this->iteration, this->movieLambda) / p.getNumberOfUsers();
        mNorm = norm_frobenius(p.getM());
        //        const Real objectiveAfterMoviePhase = mLoss + movieLambda * mNorm + userLambda * uNorm;
        assert(mLoss > 0);
        assert(mNorm > 0);
        this->movieLosses.push_back(mLoss);
        this->movieNorms.push_back(mNorm);
        this->movieLambdas.push_back(movieLambda);
        if (mLoss < minMLoss) {
            std::clog << "Found better M in iteration " << this->iteration << std::endl;
            p.storeCurrentMasBestM();
            minMLoss = mLoss;
        }
        // End of Movie Phase
        //**********************************************************************

        // Work in progress break criterion        
        //        const Real progressInMoviePhase = (objectiveBeforeMoviePhase-objectiveAfterMoviePhase)/objectiveBeforeMoviePhase;
        //        const Real progressInUserPhase  = (objectiveBeforeUserPhase-objectiveAfterUserPhase)/objectiveBeforeUserPhase;
        //        const Real progress = progressInMoviePhase + progressInUserPhase;
        //        std::cout << "progressInMoviePhase: " << progressInMoviePhase << std::endl;
        //        std::cout << "progressInUserPhase:  " << progressInUserPhase << std::endl;
        //        std::cout << "Progress:             " << progress << std::endl;
        // Work in progress break criterion        


        // Update objective functions
        const Real objectiveFunctionValue = mLoss + movieLambda * mNorm + userLambda * uNorm;
        this->objectiveFunctionValues.push_back(objectiveFunctionValue);

        ofEval->ofVal = objectiveFunctionValue;
        ofEval->mLoss = mLoss;
        ofEval->mLambda = movieLambda;
        ofEval->mNorm = mNorm;
        ofEval->uLoss = uLoss;
        ofEval->uLambda = userLambda;
        ofEval->uNorm = uNorm;

#ifndef NDEBUG
        if (p.usingMovieOffset()) {
            for (size_t row = 0; row < p.getU().size1(); ++row) {
                assert(p.getU()(row, cofi::MOVIE_OFFSET_COLUMN) == 1);
            }
        }

        if (p.usingUserOffset()) {
            for (size_t row = 0; row < p.getM().size1(); ++row) {
                assert(p.getM()(row, cofi::USER_OFFSET_COLUMN) == 1);
            }
        }

#endif

        // Do evaluations
        eval.eval(p);

    }// Main loop
    out.close();

    p.save("weak");

    if (p.getEvaluationMode() == STRONG) {
        p.switchToStrongGeneralization();
        p.restoreBestM();
        std::ofstream strongOut((outFolder + "result-strong.csv").c_str());
        CSVFileEvaluator strongEval(strongOut);
        strongEval.registerConfiguredEvaluators();
        std::clog << "COFIBMRM: User Strong Generalization Phase started" << std::endl;
        const Real uLoss = userPhase.run(p, 1, userLambda); // TODO: 1 is the wrong iteration conter here...
        const Real uNorm = norm_frobenius(p.getU());
        this->userLosses.push_back(uLoss);
        this->userNorms.push_back(uNorm);
        std::clog << "COFIBMRM: Strong UserPhase finished with a loss of " << uLoss << " and a norm of " << uNorm << std::endl;
        strongEval.eval(p);
        strongOut.close();
        p.save("strong");
    }
}
