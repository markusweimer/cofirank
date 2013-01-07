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

#include <string>
#include <vector>
#include <vector>
#include <vector>

#include "csvfileevaluator.hpp"
#include "ndcgevaluator.hpp"
#include "binaryevaluator.hpp"
#include "timeevaluator.hpp"
#include "normevaluator.hpp"
#include "meansquarederror.hpp"
#include "utils/configuration.hpp"

const static std::string s = " , ";

using std::string;
using std::map;

cofi::CSVFileEvaluator::CSVFileEvaluator(std::ostream& out):headerWritten(false), out(out){}// Constructor


void cofi::CSVFileEvaluator::registerConfiguredEvaluators(void){
    assert(!headerWritten);
    Configuration& conf = Configuration::getInstance();
    // Setup the evaluators
    registerEvaluator(new TimeEvaluator());
    
    if(conf.getIntAsBool("cofi.eval.binary")){
        registerEvaluator(new BinaryEvaluator());
    }
    if(conf.getIntAsBool("cofi.eval.ndcg")){
        registerEvaluator(new NDCGEvaluator());
    }
    if(conf.getIntAsBool("cofi.eval.norm")){
        registerEvaluator(new NormEvaluator());
    }
    if (conf.getIntAsBool("cofi.eval.mse")){
        registerEvaluator(new MSEEvaluator());
    }
    
    evaluateOnTestSet  = conf.getIntAsBool("cofi.eval.evaluateOnTestSet");
    evaluateOnTrainSet = conf.getIntAsBool("cofi.eval.evaluateOnTrainSet");
}


void cofi::CSVFileEvaluator::writeHeaders(void){
    assert(!headerWritten);
    // write the header to the csv file
    for (size_t i=0; i< dataLessEvals.size(); ++i) {
        std::vector<string> names = dataLessEvals[i]->names();
        for (size_t j=0; j < names.size(); ++j) {
            out << names[j]<< s;
        }
    }
    
    
    for (size_t i=0; i< dataEvals.size(); ++i) {
        std::vector<string> names = dataEvals[i]->names();
        if (evaluateOnTestSet){
            for (size_t j=0; j < names.size(); ++j) {
                out << "test-" << names[j]<< s;
            }
        }
        if (evaluateOnTrainSet){
            for (size_t j=0; j < names.size(); ++j) {
                out << "train-" << names[j]<< s;
            }
        }
        
    }
    out << std::endl;
    headerWritten = true;
}


void cofi::CSVFileEvaluator::eval(cofi::Problem& p) {
    if(!headerWritten){
        writeHeaders();
    }
    assert(headerWritten);
    for (size_t i=0; i< dataLessEvals.size(); ++i) {
        std::vector<string> names = dataLessEvals[i]->names();
        map<string, double> values;
        dataLessEvals[i]->eval(p, values);
        for (size_t j=0; j < names.size(); ++j) {
            out << values[names[j]]<< s;
        }
    }
    
    for (size_t i=0; i< dataEvals.size(); ++i) {
        
        std::vector<string> names = dataEvals[i]->names();
        map<string, double> values;
        if (evaluateOnTestSet){
            cofi::UserIterator iter = p.getTestIterator();
            dataEvals[i]->eval(iter, values);
            for (size_t j=0; j < names.size(); ++j) {
                out << values[names[j]]<< s;
            }
        }
        
        if (evaluateOnTrainSet){
            cofi::UserIterator iter = p.getTrainIterator();
            dataEvals[i]->eval(iter, values);
            for (size_t j=0; j < names.size(); ++j) {
                out << values[names[j]]<< s;
            }
        }
    }
    out << std::endl;
}

cofi::CSVFileEvaluator::~CSVFileEvaluator() {
    // delete the evaluators
    for (size_t i=0; i< dataEvals.size(); ++i) {
        delete dataEvals[i];
    }
    
    for (size_t i=0; i< dataLessEvals.size(); ++i) {
        delete dataLessEvals[i];
    }
    
}

void cofi::CSVFileEvaluator::registerEvaluator(DataIndependentEvaluator* eval){
    assert(!headerWritten);
    dataLessEvals.push_back(eval);
}
void cofi::CSVFileEvaluator::registerEvaluator(CofiEvaluator* eval){
    assert(!headerWritten);
    dataEvals.push_back(eval);
}
