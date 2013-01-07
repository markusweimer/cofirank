#ifndef _COFIEXCEPTION_HPP_
#define _COFIEXCEPTION_HPP_
#include "cofiexception.hpp"
#include <string>

namespace cofi {

    class CoFiException {
    public:


        CoFiException() {
        };
        CoFiException(const std::string& message);


        virtual ~CoFiException() {
        };
        virtual std::string describe(void);

    protected:
        const std::string message;
    };

    class NotImplementedException : public CoFiException {
    public:


        NotImplementedException(void) : CoFiException() {
        }


        NotImplementedException(const std::string& message) : CoFiException(message) {
        }
    };

    class InvalidParameterException : public CoFiException {
    public:


        InvalidParameterException(void) : CoFiException() {
        }


        InvalidParameterException(const std::string& message) : CoFiException(message) {
        }
    };

    class NumericException : public CoFiException {
    public:


        NumericException(void) : CoFiException() {
        }


        NumericException(const std::string& message) : CoFiException(message) {
        }

    };
}
#endif /* _COFIEXCEPTION_HPP_ */
