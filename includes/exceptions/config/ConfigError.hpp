#pragma once
#include <exception>

namespace webserv {
    namespace exceptions {
        namespace config {
            /**
             * @brief Exception thrown when file name is invalid
            */
            class ConfigError : public std::exception {
                public:
                    virtual const char* what() const throw() {
                        return "Found error(s) in configuration file. Please check configLogs file or use ConfigFile::getErrors() method";
                    }
            };
        }
    }
}
