#ifndef _DOMAINMODELFACTORY_HPP_
#define _DOMAINMODELFACTORY_HPP_

#include <cassert>
#include "cofilossfunction.hpp"

class LossFunctionFactory {
public:

    enum ModelEnum {
        NDCG, REGRESSION, ORDINAL
    };

    CofiLossFunction* get(ublas::matrix<Real>& X, ublas::matrix<Real>& Y);

    /**
     * @return a reference to the current instance.
     */
    static LossFunctionFactory& getInstance(void);

private:
    LossFunctionFactory(void);


    LossFunctionFactory(const LossFunctionFactory& dmf) {
        assert(false);
    }


    static LossFunctionFactory* instance;
    ModelEnum m;
};

#endif /* _DOMAINMODELFACTORY_HPP_ */
