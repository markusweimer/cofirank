//
// File:   graphkernellosswrapper.h
// Author: weimer
//
// Created on January 29, 2008, 2:38 PM
//

#ifndef _GRAPHKERNELLOSSWRAPPER_H
#define	_GRAPHKERNELLOSSWRAPPER_H
#include "core/types.hpp"
#include "bmrm/lossfunction.hpp"
#include "cofi/problem.hpp"

namespace cofi{
    class GraphKernelLossWrapper : public LossFunction{
    public:
        GraphKernelLossWrapper(Problem& p);
        void ComputeLossGradient(WType& w, Real &loss, cofi::WType& grad);
    private:
        typedef cofi::DType::const_iterator1 itr1;
        typedef cofi::DType::const_iterator2 itr2;
        Problem& p;
    };
}


#endif	/* _GRAPHKERNELLOSSWRAPPER_H */
