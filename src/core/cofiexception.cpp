#include "cofiexception.hpp"


cofi::CoFiException::CoFiException(const std::string& message) : message(message) {
}


std::string cofi::CoFiException::describe() {
    return this->message;
}
