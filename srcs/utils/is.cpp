#include "utils.hpp"
#include <unistd.h>

using namespace webserv::utils;

/**
 * @brief Checks if a file is ...
 * 
 * @param flag READABLE, WRITABLE, EXECUTABLE, FILE
 * @param ref Path to a file
 * @return bool
 */
namespace webserv {
    namespace utils {
        bool	is(int flag, const std::string &ref)
        {
            switch (flag)
            {
                case AccessFlags::READABLE:
                    return (access(ref.c_str(), R_OK) == 0);
                case AccessFlags::WRITABLE:
                    return (access(ref.c_str(), W_OK) == 0);
                case AccessFlags::EXECUTABLE:
                    return (access(ref.c_str(), X_OK) == 0);
                case AccessFlags::FILE:
                    return (access(ref.c_str(), F_OK) == 0);
                default:
                    return false;
            }
        }
    }
}
