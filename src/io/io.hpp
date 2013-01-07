#include <fstream>

#include "svmlightreader.hpp"


namespace cofi {
    namespace io {


        /**
         * Stores the given matrix in a file with the given name
         *
         * @param m the matrix to store
         * @param filename the name of the file to write the matrix to
         */
        template<class M> void storeMatrixOld(const M& m, std::string filename) {
            std::ofstream f(filename.c_str());
            for (size_t row = 0; row < m.size1(); row++) {
                for (size_t column = 0; column < m.size2(); column++) {
                    typename M::value_type value = m(row, column);
                    if (value != 0) {
                        // +1, as the files all start with column 1
                        f << column + 1 << ":" << value << " ";
                    }
                }
                f << std::endl;
            }
            f.close();
        }


        /**
         * Stores the given matrix in a file with the given name
         *
         * @param m the matrix to store
         * @param filename the name of the file to write the matrix to
         */
        template<class M> void storeMatrix(const M& m, std::string filename) {
            std::ofstream f(filename.c_str());
            for (typename M::const_iterator1 it1 = m.begin1(); it1 != m.end1(); ++it1) {
                for (typename M::const_iterator2 it2 = it1.begin(); it2 != it1.end(); ++it2) {
                    if (*it2 == 0) {
                        continue;
                    }
                    f << it2.index2() + 1 << ":" << (*it2) << " ";
                }
                f << std::endl;
            }
            f.close();
        }


        template <class M> std::pair<size_t, size_t> getDimensions(const std::string& filename) {
            SVMLightReader<M> reader(filename);
            IndexValueScanner scanner;
            reader.process(scanner);
            return std::pair<size_t, size_t > (scanner.size1(), scanner.size2());
        };


        /**
         * Loads a matrix from a libsvm file.
         *
         * The matrix will be resized appropriately
         *
         * @param m the matrix to be loaded
         * @param filename the filename to read the matrix from;
         */
        template<class M> void resizeAndLoadMatrix(M& m, const std::string& filename) {
            std::pair<size_t, size_t> dims = getDimensions<M > (filename);
            m.resize(dims.first, dims.second, false);
            IndexValueInserter<M> inserter(m);
            SVMLightReader<M> reader(filename);
            reader.process(inserter);
        }


        /**
         * Loads a matrix from a libsvm file.
         *
         * THIS ASSUMES THAT THE MATRIX IS OF THE RIGHT SIZE
         *
         * @param m the matrix to be loaded
         * @param filename the filename to read the matrix from;
         */
        template<class M> void loadMatrixWithOutResize(M& m, const std::string& filename) {
            IndexValueInserter<M> inserter(m);
            SVMLightReader<M> reader(filename);
            reader.process(inserter);
        };


        template<class M> void loadMatrixConstrained(M& m, const std::string& filename) {
            ConstrainedIndexValueInserter<M> inserter(m);
            SVMLightReader<M> reader(filename);
            reader.process(inserter);
        };


    }
}