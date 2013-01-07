#include "configexception.hpp"
#include <iostream>
std::string cofi::ConfigException::describe(void) {
    return "Configuration problem. Unable to find the configuration option "
            + this->key;
}

cofi::ConfigException::ConfigException(const std::string& _key) : key(_key) {
    std::clog << "ConfigException: Unable to find the configuration option " << _key << std::endl;
}
