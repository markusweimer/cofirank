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
 * Created      : 10/10/2007
 *
 * Last Updated :
 */

#include "binaryevaluator.hpp"

#include "cofi/useriterator.hpp"

static std::string FP  = "False Positives";
static std::string FN  = "False Negatives";
static std::string CC  = "Correctly Classified";
static std::string PP  = "Predicted Positive";
static std::string PN  = "Predicted Negatives";
static std::string ALL = "All classifications";
static std::string ACC = "Accuracy";



cofi::BinaryEvaluator::BinaryEvaluator(void){
    
}

cofi::BinaryEvaluator::~BinaryEvaluator(void){
    
}

void cofi::BinaryEvaluator::eval(cofi::UserIterator& iter, std::map<std::string, double>& results){
    unsigned int falsePositives = 0;
    unsigned int falseNegatives = 0;
    unsigned int predictedPositives = 0;
    unsigned int predictedNegatives = 0;
    unsigned int correct = 0;
    unsigned int all = 0;
    while (iter.hasNext()) {
        
        iter.advance();
        ublas::matrix<Real>& Y = iter.getY();
        ublas::matrix<Real> f = ublas::matrix<Real>(Y.size1(), Y.size2());
        iter.predict(f);
        assert(Y.size1() == f.size1());
        
        for(size_t i=0; i<Y.size1(); ++i){
            if(f(i, 0) < 0){
                predictedNegatives += 1;
            }
            else{
                predictedPositives += 1;
            }
            
            if(f(i, 0) < 0 && Y(i, 0) >= 0){
                falseNegatives += 1;
            }
            else if(f(i, 0) >= 0 && Y(i, 0) < 0){
                falsePositives += 1;
            }
            else{
                correct += 1;
            }
            all += 1;
        }
        
    }
    
    assert(falsePositives + falseNegatives + correct == all);
    
    results[FP]  = static_cast<double>(falsePositives);
    results[FN]  = static_cast<double>(falseNegatives);
    results[CC]  = static_cast<double>(correct);
    results[PP]  = static_cast<double>(predictedPositives);
    results[PN]  = static_cast<double>(predictedNegatives);
    results[ALL] = static_cast<double>(all);
	results[ACC] = results[CC] / results[ALL];
}


std::vector<std::string> cofi::BinaryEvaluator::names(void){
    std::vector<std::string> result;
    result.push_back(FP);
    result.push_back(FN);
    result.push_back(CC);
    result.push_back(PP);
    result.push_back(PN);
    result.push_back(ALL);
    result.push_back(ACC);
    return result;
}
