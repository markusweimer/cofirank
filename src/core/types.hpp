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
 * Created      : 22/05/2007
 *
 * Last Updated :
 */


#ifndef __TYPES_HPP__
#define __TYPES_HPP__

// Boost
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_sparse.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <boost/numeric/ublas/vector.hpp>



namespace ublas = boost::numeric::ublas;
/**
 * The type for real values
 */
typedef double Real; // The type of floats we are using

namespace cofi {
    // The type used for the entries in the matrix to be approximated
    typedef Real EntryType;

    // Types for the input data
    typedef ublas::compressed_matrix<cofi::EntryType> DType;
    typedef ublas::compressed_matrix<Real> SType;

    typedef ublas::matrix<Real> FType;


    // The learned matrices
    typedef ublas::matrix<Real> MType;
    typedef ublas::matrix<Real> UType;

    // The types for the small optimization problems
    typedef ublas::matrix<Real> XType;
    typedef ublas::matrix<Real> YType;
    typedef ublas::matrix<Real> WType;


    const size_t USER_OFFSET_COLUMN  =  0;
    const size_t MOVIE_OFFSET_COLUMN =  1;
    const size_t NOVELTY_RHO_COLUMN  =  2;

    typedef size_t userid;
    typedef size_t itemid;
}

#endif
