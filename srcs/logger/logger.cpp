#include "logger.hpp"
#include <unistd.h>
#include <iostream>
#include <fstream>

# define DEBUG_COLOR "\033[38;5;250m"
# define INFO_COLOR "\033[38;5;45m"
# define WARNING_COLOR "\033[38;5;214m"
# define ERROR_COLOR "\033[38;5;196m"
# define FATAL_COLOR "\033[38;5;160m"
# define RESET "\033[0m"

using namespace webserv::logger;

static std::string _logFile = "";
static int _logLevel = LogLevel::Info;
static std::ofstream _logStream;
static bool _logStreamOpen = false;

namespace webserv {
    namespace logger {
        void log(int level, std::string const& message) {
            if (level < _logLevel)
                return;
            if (!_logStreamOpen && !_logFile.empty()) {
                _logStream.open(_logFile.c_str(), std::ios::out | std::ios::app | std::ios::trunc );
                _logStreamOpen = true;
            }
            switch (level) {
                case LogLevel::Debug:
                    std::cerr << DEBUG_COLOR << "[DEBUG] " << message << RESET << std::endl;
                    _logStream << "[DEBUG] " << message << std::endl;
                    _logStream.flush();
                    break;
                case LogLevel::Info:
                    std::cerr << INFO_COLOR << "[INFO] " << message << RESET << std::endl;
                    _logStream << "[INFO] " << message << std::endl;
                    _logStream.flush();
                    break;
                case LogLevel::Warning:
                    std::cerr << WARNING_COLOR << "[WARN] " << message << RESET << std::endl;
                    _logStream << "[WARN] " << message << std::endl;
                    _logStream.flush();
                    break;
                case LogLevel::Error:
                    std::cerr << ERROR_COLOR << "[ERROR] " << message << RESET << std::endl;
                    _logStream << "[ERROR] " << message << std::endl;
                    _logStream.flush();
                    break;
                case LogLevel::Fatal:
                    std::cerr << FATAL_COLOR << "[FATAL] " << message << RESET << std::endl;
                    _logStream << "[FATAL] " << message << std::endl;
                    _logStream.flush();
                    _logStream.close();
                    _exit(1);
                    break;
            }
        }

        void debug(std::string const& message)               { log(LogLevel::Debug, message); }
        void info(std::string const& message)                { log(LogLevel::Info, message); }
        void warning(std::string const& message)             { log(LogLevel::Warning, message); }
        void error(std::string const& message)               { log(LogLevel::Error, message); }
        void fatal(std::string const& message)               { log(LogLevel::Fatal, message); }
        void setLevel(int level)                        { _logLevel = level; }
        void setOutputFile(std::string const& path)          { _logFile = path; }
        void closeOutputFile()                               { _logStream.close(); }
    }
}
