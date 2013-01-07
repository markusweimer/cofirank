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
 * Created      : 04/06/2007
 *
 * Last Updated :
 */

#include "lossfunctionfactory.hpp"
#include "utils/configuration.hpp"
#include "core/cofiexception.hpp"

#include <string>

// The domain Models
#include "loss/ndcgdomainmodel.hpp"
#include "loss/leastsquaredomainmodel.hpp"
#include "loss/preferencerankingdomainmodel.hpp"

LossFunctionFactory::LossFunctionFactory(void) {
    Configuration& conf = Configuration::getInstance();
    std::string name = conf.getString("cofi.loss");
    if (name == "NDCG") {
        std::clog << "DomainModelFactory::DomainModelFactory: Using NDCG" << std::endl;
        m = NDCG;
    } else if (name == "REGRESSION") {
        std::clog << "DomainModelFactory::DomainModelFactory: Using REGRESSION" << std::endl;
        m = REGRESSION;
    } else if (name == "ORDINAL") {
        std::clog << "DomainModelFactory::DomainModelFactory: Using ORDINAL" << std::endl;
        m = ORDINAL;
    } else {
        throw cofi::ConfigException("DomainModelFactory: No Domain Model choosen in the configuration!");
    }
}


CofiLossFunction * LossFunctionFactory::get(ublas::matrix<Real>& X, ublas::matrix<Real>& Y) {
    if (X.size1() != Y.size1()) {
        throw cofi::InvalidParameterException("X and Y differ in the number of rows.");
    }
    switch (m) {
        case NDCG:
            return new NDCGDomainModel(X, Y);

        case REGRESSION:
            return new LeastSquareDomainModel(X, Y);

        case ORDINAL:
            return new PreferenceRankingDomainModel(X, Y);

        default:
            throw cofi::InvalidParameterException("DomainModelFactory::get(): unable to create a domain model.");
    }

}

// Singleton implementation
LossFunctionFactory * LossFunctionFactory::instance = NULL;


LossFunctionFactory & LossFunctionFactory::getInstance(void) {
    if (instance == NULL)
        instance = new LossFunctionFactory();
    return *instance;
}