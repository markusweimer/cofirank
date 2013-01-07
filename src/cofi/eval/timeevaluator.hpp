//
// File:   timeevaluator.h
// Author: weimer
//
// Created on December 13, 2007, 5:07 PM
//

#ifndef _TIMEEVALUATOR_H
#define	_TIMEEVALUATOR_H

#include "dataindependentevaluator.hpp"
#include "utils/timer.hpp"
namespace cofi{
    class TimeEvaluator: public DataIndependentEvaluator{
    public :
        
        TimeEvaluator(void);
        ~TimeEvaluator(void) {};
        
        /**
         * @return the name of the evaluation measure
         */
        std::vector<std::string> names(void);
        
        /**
         * Evaluates based on current state of problem and writes to the stream.
         * @param p the problem which holds the data to evaluate on.
         * @return the evaluation result.
         */
        void eval(cofi::Problem& p, std::map<std::string, double>& results);
        
    private:
        CTimer timer;
        size_t iteration;
    };
    
}
#endif	/* _TIMEEVALUATOR_H */

