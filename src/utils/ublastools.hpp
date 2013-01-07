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
 * Last Updated :
 */
#ifndef _UBLASTOOLS_HPP_
#define _UBLASTOOLS_HPP_
#include <ext/numeric>         // for iota
#include <cstdlib>
#include "core/types.hpp"

namespace cofi {
    namespace ublastools {


        /**
         * Initializes the given Matrix with random numbers
         *
         * @param M the type of the matrix
         * @param m the matrix to be initialized.
         * @param factor a factor which is used to multiply the random number with
         * @param offset a number which is added to the random number
         */
        template<class M> void random(M& m, const double factor = 1.0, const double offset = 0.0) {
            const size_t rows = m.size1();
            const size_t cols = m.size2();

            for (size_t row = 0; row < rows; ++row) {
                for (size_t col = 0; col < cols; ++col) {
                    const Real r = ((Real) rand()) / RAND_MAX;
                    //const Real r = drand48();
                    assert(r >= 0.0);
                    assert(r <= 1.0);
                    m(row, col) = r * factor + offset;
                }
            }
        }


        /**
         * Initializes the given Matrix with random numbers and resizes it.
         *
         * @param M the type of the matrix
         * @param m the matrix to be initialized.
         * @param factor a factor which is used to multiply the random number with
         * @param offset a number which is added to the random number
         */
        template<class M> void randomResize(M& m, const size_t rows, const size_t cols, const double factor = 1.0, const double offset = 0.0) {
            m.resize(rows, cols, false);
            for (size_t row = 0; row < rows; ++row) {
                for (size_t col = 0; col < cols; ++col) {
                    const Real r = ((Real) rand()) / RAND_MAX;
                    //const Real r = drand48();
                    assert(r >= 0.0);
                    assert(r <= 1.0);
                    m(row, col) = r * factor + offset;
                }
            }
        }


        /**
         * @return true, if all entries in the two matrices are equal and the matrices
         *         have the same size
         */
        template<class M> bool allEqual(const M& m1, const M& m2) {
            if (m1.size1() != m2.size1())
                return false;
            if (m1.size2() != m2.size2())
                return false;

            for (size_t i = 0; i < m1.size1(); ++i) {
                for (size_t j = 0; i < m1.size2(); ++j) {
                    if (m1(i, j) != m2(i, j))
                        return false;
                }
            }
            return true;
        }


        /**
         * A inner product that ignores some elements of v2.
         *
         * This code assumes that v2 has (ignoreEnd-ignoreStart+1) more elements 
         * than v1.
         *
         * @param v1 the first parameter to the inner product.
         * @param v2 the second parameter to the inner product.
         * @param ignoreStart the first element in v2 to be ignored. (inclusive)
         * @param ignoreEnd the first element of v2 that is no longer ignored (exclusive)
         *
         */
        template<class V1, class V2> double inner_prod_ignore(const V1& v1, const V2& v2, size_t ignoreStart, size_t ignoreEnd) {
#ifndef NDEBUG
            std::clog << "cofi::ublastools::inner_prod_ignore(): v1.size(): " << v1.size() << " v2.size(): " << v2.size() << " ignoreStart: " << ignoreStart << "ignoreEnd: " << ignoreEnd << std::endl;
            const size_t numberOfIgnoredElements = ignoreEnd - ignoreStart;
            assert(v2.size() - numberOfIgnoredElements == v1.size());
#endif

            size_t i1 = 0;
            size_t i2 = 0;

            double result = 0.0;
            while (i1 < v1.size() && i2 < v2.size()) {
                if (i2 >= ignoreStart && i2 <= ignoreEnd) {
                    i2 = ignoreEnd + 1;
                }
                result += v1(i1) * v2(i2);
                ++i1;
                ++i2;
            }

            return result;

        }

        /**
         * A comparator that compares two indizes by looking up the corresponding
         * values in a matrix.
         *
         * In the ranking losses, we need to find a sorted index vector for vectors.
         * To make matters worse, the vectors are stored in the first column of a
         * matrix.
         *
         * This class implements a comparator for a decreasing sort of such a vector.
         *
         */
        template<typename M> class DecreasingIndexComparator {
        public:


            DecreasingIndexComparator(const M& m) :
            theMatrix(m) {
            }


            inline bool operator()(const size_t& a, const size_t& b) {
                return theMatrix(a, 0) > theMatrix(b, 0);
            }

        private:
            const M& theMatrix;
        };

        /**
         * A comparator that compares two indizes by looking up the corresponding
         * values in a vector.
         *
         * In the ranking losses, we need to find a sorted index vector for vectors.
         * To make matters worse, the vectors are stored in the first column of a
         * matrix.
         *
         * This class implements a comparator for a decreasing sort of such a vector.
         *
         */
        template<typename M> class DecreasingIndexComparatorV {
        public:


            DecreasingIndexComparatorV(const M& m) :
            theVector(m) {
            }


            inline bool operator()(const size_t& a, const size_t& b) {
                return theVector(a) > theVector(b);
            }

        private:
            const M& theVector;
        };


        /**
         * Computes a sorted index for the first column of a matrix.
         */
        template<typename M> boost::numeric::ublas::vector<size_t> decreasingSort(const M& m) {
            boost::numeric::ublas::vector<size_t> result(m.size1());
            for (size_t i = 0; i < result.size(); ++i) {
                result[i] = i;
            }
            DecreasingIndexComparator<M> comp(m);
            sort(result.begin(), result.end(), comp);
            return result;
        }


        template<typename V> boost::numeric::ublas::vector<size_t> decArgSort(const V& v) {
            boost::numeric::ublas::vector<size_t> result(v.size());
            for (size_t i = 0; i < result.size(); ++i) {
                result[i] = i;
            }
            DecreasingIndexComparatorV<V> comp(v);
            sort(result.begin(), result.end(), comp);
            return result;
        }


        template<typename V> bool isDecreasinglySorted(const V&v, const boost::numeric::ublas::vector<size_t> pi) {
            assert(pi.size() == v.size());
            double lastValue = std::numeric_limits<double>::infinity();
            for (size_t i = 0; i < v.size(); ++i) {
                if (v(pi(i)) > lastValue) {
                    return false;
                }
                lastValue = v(pi(i));
            }
            return true;
        }


        template<typename M> Real inner_prod(const M& m1, const M& m2) {
            assert(m1.size1() == m2.size1() && m1.size2() == m2.size2());
            Real result = 0;
            for (size_t row = 0; row < m1.size1(); ++row) {
                for (size_t col = 0; col < m1.size2(); ++col) {
                    result += m1(row, col) * m2(row, col);
                }
            }
            return result;
        }


        /**
         * Computes the inverse permutation of pi
         */
        boost::numeric::ublas::vector<size_t> invert_permutation(const boost::numeric::ublas::vector<size_t>& pi);


        /**
         * Computes the variance of the values in matrix.
         */
        template<typename M> Real var(const M& matrix) {
            const ublas::scalar_matrix<Real> m(matrix.size1(), matrix.size2(), mean(matrix));
            return norm_frobenius(matrix - m);
        }

        /**
         * Computes the mean of the values in matrix.
         */
        Real mean(const ublas::matrix<Real>& M);

        /**
         * Computes the mean of the values in matrix.
         */
        Real mean(const ublas::compressed_matrix<Real>& M);


    }
}// Namespace



#endif /* _UBLASTOOLS_HPP_ */
