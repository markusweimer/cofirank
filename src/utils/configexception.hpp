#ifndef _CONFIGEXCEPTION_HPP_
#define _CONFIGEXCEPTION_HPP_

#include <string>
#include <core/cofiexception.hpp>

namespace cofi {

    class ConfigException : public CoFiException {
    public:
        ConfigException(const std::string& key);


        virtual ~ConfigException(void) {
        };

        virtual std::string describe(void);

    private:
        const std::string key;

    };
}
#endif /* _CONFIGEXCEPTION_HPP_ */
