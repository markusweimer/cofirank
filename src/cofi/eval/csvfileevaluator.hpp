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
#ifndef _CSVFILEEVALUATOR_HPP_
#define _CSVFILEEVALUATOR_HPP_
#include <vector>
#include <string>
#include <map>

#include "cofi/eval/cofievaluator.hpp"
#include "cofi/eval/dataindependentevaluator.hpp"
#include "cofi/problem.hpp"
#include "cofi/eval/timeevaluator.hpp"

namespace cofi{
    /**
     * Evaluates the current state and writes results to a CSV file.
     *
     *
     * We distinguish two kinds of Evaluators: Those which depend on the actual
     * test data and those who don't. The latter are derived from
     * DataIndependentEvaluator, the former from CofiEvaluator.
     *
     *
     */
    class CSVFileEvaluator {
    public:
        
        /**
         * Writes the CSV header to the given stream and initializes the
         * evaluators.
         *
         * @param out the stream to write the data to.
         */
        CSVFileEvaluator(std::ostream& out);
        
        /**
         * Destructor. Mainly deletes the evaluators.
         * DOES NOT CLOSE THE STREAM
         */
        ~CSVFileEvaluator();
        
        /**
         * Registers all the evaluators configured in the config file.
         */
        void registerConfiguredEvaluators(void);
        
        /**
         * Registers the given evaluator
         *
         * This class takes ownership of the given object and will destroy it 
         * upon destruction of the CSVFileEvaluator.
         */
        
        void registerEvaluator(DataIndependentEvaluator* eval);
        void registerEvaluator(CofiEvaluator* eval);
        
        
        /**
         * Applys the evaluators to the current state and writes the results
         * to the stream.
         *
         * @param p the Problem to evaluate.
         */
        void eval(cofi::Problem& p);
        
        
    private:
        
        /**
         * The evaluators
         */
        std::vector<CofiEvaluator*> dataEvals;
        std::vector<DataIndependentEvaluator*> dataLessEvals;
        
        /**
         * Writes the headers to the outstream.
         */
        void writeHeaders(void);
        
        /**
         * True, if the header has already been written.
         */
        bool headerWritten;
        
        /**
         * The stream to write too
         */
        std::ostream& out;
        
        bool evaluateOnTestSet;
        bool evaluateOnTrainSet;
    };
}
#endif
