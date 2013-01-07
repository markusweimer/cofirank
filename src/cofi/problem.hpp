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
 * Created      : 04/07/2007
 *
 * Last Updated :
 */
#ifndef _NEWPROBLEM_HPP_
#define _NEWPROBLEM_HPP_

#include "core/types.hpp"
#include "useriterator.hpp"
#include <boost/numeric/ublas/vector.hpp>
#include <string>
#include <iostream>

namespace ublas = boost::numeric::ublas;

namespace cofi{
    
    // Forward declaration.
    class UserIterator;
    
    enum EvaluationMode{STRONG, WEAK};
    /**
     * All the information about a collaborative filtering instance.
     *
     * This class is the general hub that has all the information that 
     * distinguishes one Cofirank run from another. This includes:
     *
     * - The train and test matrices. They are called Y in the papers and D here. Sorry.
     * - The result matrices U,M and A
     * - The weight matrix S used in the graph kernel extension
     * 
     * In addition, this class has facilities for certain extensions. For 
     * example, it can set the last column of M to zero or one for the offsets.
     *
     *
     */
    class Problem  {
    public:
        
        /**
         * Initializes the problem.
         *
         * This will load the datasets for training and testing as well as
         * initialize all other data structures.
         *
         */
        Problem(void);
        
        /**
         * Will delete D, M, U
         */
        ~Problem(void);
        
        /**
         * @return a UserIterator over the training data
         */
        UserIterator getTrainIterator();
        
        /**
         * @return a UserIterator over the test data
         */
        UserIterator getTestIterator();
        
        /**
         * @return a reference to the matrix S, where
         *         S[i,j]=1/n iff user j rated item j. n is the number of items
         *         rated by that user.
         */
        cofi::SType& getS(void) { assert(usingGraphKernel()); return *S; }
        
        /**
         * @return a reference to the matrix A which is used when training with
         *         graph kernels
         */
        cofi::MType& getA(void) { assert(usingGraphKernel()); return *A; }
        
        /**
         * @return a reference to the test Matrix
         *         D[i,j] is the rating of item j by user i
         */
        cofi::DType& getTestD(void) { return *testD; }
        
        /**
         * @return a reference to the train Matrix.
         *         D[i,j] is the rating of item j by user i
         */
        cofi::DType& getTrainD() { return *trainD; }
        
        /**
         * @return a reference to U.
         */
        cofi::UType& getU(void) {return *U;}
        
        /**
         * @return a reference to M.
         */
        cofi::MType& getM(void) { return *M; }
        
        /**
         * Switches to strong generalization phase.
         *
         * rereads the train- and testmatrix for strong generalization and
         * reads or creates a new U.
         *
         */
        void switchToStrongGeneralization(void);
        
        /**
         * Sets the last column of M to 0/1. This is used when we have an offset.
         */
        void setUserOffsetColumnInMToZero(void);
        void setUserOffsetColumnInMToOne(void);
        
        void setMovieOffsetColumnInUToOne(void);
        
        
        /**
         * @return the evaluation mode. Either STRONG or WEAK
         */
        EvaluationMode getEvaluationMode(void){return evalMode;};
         
        /**
         * @return true, if the adaptive regularization is used.
         */
        bool usingAdaptiveRegularization(void){return useAdaptiveRegularization;}
        
        /**
         * @return true, if the graph kernel shall be applied.
         */
        bool usingGraphKernel(void){return useGraphKernel;}
        
        /**
         * @return the weight for the row i in U. Used for the adaptive regulariser.
         */
        Real getWeightForU(const size_t i){return weightsU[i];};
        
        /**
         * @return the number of features to learn.
         */
        size_t getDimW(void){return dimW;}
        
        /**
         * @return the number of items.
         */
        size_t getNumberOfItems(void){return nMovies;}
        
        /**
         * @return the number of users.
         */
        size_t getNumberOfUsers(void){return getU().size1();}
        
        /**
         * @return true, if the movie offset is enabled.
         */
        inline bool usingMovieOffset(void){return useMovieOffset;}
        
        /**
         * @return true, if the user offset is enabled.
         */
        inline bool usingUserOffset(void){return useUserOffset;}
        
        
        
        
        /**
         * Save the resulting matrices to disk into the folder given by the configuration.
         */
        void save(std::string prefix);
        
        
        void storeCurrentMasBestM(void){
            if(bestM) delete bestM;
            bestM = new MType(M->size1(), M->size2());
            for(size_t i=0; i < M->size1();++i){
                for(size_t j=0; j < M->size2();++j){
                    (*bestM)(i, j) = (*M)(i, j);
                }
            }
        }
        
        void restoreBestM(void){
            if(M) delete M;
            M = bestM;
            bestM = NULL;
        }
        
    private:
        bool useMovieOffset;            // Whether or not to use the movie offset
        bool useUserOffset;             // Whether or not to use the user offset
        bool useGraphKernel;            // Graph Kernel trick
        bool useAdaptiveRegularization; // Adaptive regularization trick
        
        /**
         * Config options
         */
        EvaluationMode evalMode;        // STRONG or WEAK
        size_t dimW;                    // Number of features in U,A and M
        
        /**
         * Input matrices
         */
        cofi::DType* trainD;            // Train ratings
        cofi::DType* testD;             // Test ratings
        cofi::SType* S;                 // S[i,j] is 1 iff the user i saw movie j
        
        
        /**
         * Learned matrices
         */
        cofi::UType*  U;                // User features
        cofi::MType*  M;                // Movie features
        cofi::MType*  A;                // TODO
        cofi::MType*  bestM;            // Best movie features
        
        
        /**
         * Precomputed helper variables
         */
        size_t nMovies;                        // Number of movies (max over train and test)
        ublas::vector<Real> weightsU;          // weightsU[i] := The weights for user i
        
        /**
         * Reads D from the filesystem.
         */
        void setupD(void);
        
        /**
         * Computes the matrix trainS and testS for the data
         */
        void setupS(void);
        
        /**
         * Computes the weights for the users.
         */
        void computeWeights(const Real exponent);
        
        
        
    };
}
#endif /* _NEWPROBLEM_HPP_ */
