#pragma once

#include <string>
#include <unistd.h>

namespace webserv {
    namespace logger {
        namespace LogLevel {
            enum LogLevel
            {
                Debug,
                Info,
                Warning,
                Error,
                Fatal
            };
        }
        void debug(std::string const &);
        void info(std::string const &);
        void warning(std::string const &);
        void error(std::string const &);
        void fatal(std::string const &);

        void setLevel(int level);
        void setOutputFile(const std::string& path);
    }
}
