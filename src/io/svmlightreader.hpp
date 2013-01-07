#ifndef _SVMLIGHTREADER_HPP_
#define _SVMLIGHTREADER_HPP_

#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "core/types.hpp"
#include "core/cofiexception.hpp"
namespace cofi {
    /**
     * Code to read SVMLight files.
     *
     * The general idea is the folllowing:
     * SVMLightReader::process() runs thru the file and generates [row, column, value] triples.
     * Note that column is the one as found in the file, e.g. it starts with 1 instead of 0.
     * These triples are then passed to a Handler that incherits IndexValueHandler.
     * In this file, you will find three of them:
     *
     * IndexValueScanner: This class just collects the biggest row and column it has ever found.
     *                    It can give you the correct dimensions for a matrix to store the contents
     *                    of the file using the size1() and size2() methods.
     *                    It will apply offset conversion for the columns to do so.
     * IndexValueInserter: This class inserts the given triple into a matrix. It applies offset
     *                     transformation to the columns in this process.
     *
     * IndexValueInserterNZ: Same, but also fills a nonzero index.
     */

    namespace io {

        /**
         * Base class for Index and Value Handlers.
         *
         * These are classes that can act upon a stream of .handle(row, col, value) events
         */
        class IndexValueHandler {
        public:
            virtual void handle(const size_t row, const size_t column, const Real value) = 0;
            virtual void handleEmptyRow(const size_t row) = 0;


            virtual ~IndexValueHandler(void) {
            }

        };

        /**
         * An index and value scanner that determines the max size of the data
         */
        class IndexValueScanner : public IndexValueHandler {
        public:


            IndexValueScanner(void) {
                maxRow = 0;
                maxCol = 0;
            }


            void handle(const size_t row, const size_t column, const Real value) {
                if (row > maxRow) {
                    maxRow = row;
                }
                if (column > maxCol) {
                    maxCol = column;
                }
            }


            void handleEmptyRow(const size_t row) {
                if (row > maxRow) {
                    maxRow = row;
                }
            }


            size_t size1() {
                return maxRow + 1;
            }


            size_t size2() {
                return maxCol + 1;
            }

        private:
            size_t maxRow;
            size_t maxCol;
        };

        /**
         * An index and value handler that inserts elements into a matrix.
         */

        template<class M> class IndexValueInserter : public IndexValueHandler {
        public:


            IndexValueInserter(M& m) :
            m(m) {
            }


            void handle(const size_t row, const size_t column, const Real value) {
                if (column < 0 || row < 0 || column >= m.size2() || row >= m.size1()) {
                    std::clog << "cofi::IndexValueInserter::handle(): row=" << row << "col=" << column << "m.size1()=" << m.size1() << "m.size2()=" << m.size2() << std::endl;
                    throw CoFiException("cofi::IndexValueInserter::handle(): Index violation.");
                }
                m(row, column) = value;
            }


            void handleEmptyRow(const size_t row) {
                assert(row < m.size1());
            }


        private:
            M& m;
        };

        /**
         * This handler will only insert those elements whose indices fit within the matrix.
         */
        template<class M> class ConstrainedIndexValueInserter : public IndexValueHandler {
        public:


            ConstrainedIndexValueInserter(M& m) :
            m(m) {
            }


            virtual void handle(const size_t row, const size_t column, const Real value) {
                if (column >= m.size2() || row >= m.size1()) {
                    return;
                }
                m(row, column) = value;
            }


            void handleEmptyRow(const size_t row) {
            }
        private:
            M& m;
        };

        /**
         * Reads svm light files into uBLASS matrizes
         *
         * The data is read as a double one value after another and this double is
         * converted to M::value_type by a cast.
         *
         * @param M the full type of the matrix, e.g. compressed_matrix<double>
         */
        template<class M> class SVMLightReader {
        public:


            /**
             * @param filename the name of the file to be read.
             */
            SVMLightReader(const std::string& filename) :
            filename(filename) {

            }
            void process(IndexValueHandler& handler);
        private:
            const std::string filename;
        };


        template<class M> void SVMLightReader<M>::process(IndexValueHandler& handler) {
            std::ifstream f(filename.c_str());
            if (!f.good()) {
                throw CoFiException("Unable to open SVMLight file: " + this->filename);
            }
            for (size_t row = 0; !f.eof(); ++row) {
                std::string line;
                getline(f, line);
                if (line.length() == 0 && !f.eof()) {
                    handler.handleEmptyRow(row);
                }
                std::istringstream iss(line);
                while (!iss.eof()) {
                    std::string token;
                    iss >> token;
                    if (token.length() == 0) {
                        continue;
                    }

                    int column = -1;
                    double value = 0.0;
                    sscanf(token.c_str(), "%d:%lf", &column, &value);
                    if (column < 0) {
                        throw CoFiException("Found a column index which is less than zero.");
                    }
                    const size_t col = static_cast<size_t> (column);
                    handler.handle(row, col-1, value);
                }
            }
            f.close();
        }
    }
}

#endif /* _SVMLIGHTREADER_HPP_ */
