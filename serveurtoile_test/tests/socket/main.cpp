#include "exceptions.hpp"
#include "Socket.hpp"
#include <iostream>
#include <vector>
#include <cstdlib>

#define RED2 "\033[0;31m"
#define GREEN2 "\033[0;32m"
#define MAGENTA2 "\033[0;35m"
#define RESET "\033[0m"
#define DOT "●"

std::vector<int> failedTests;

void printVerdict(bool success) {
    static int testNumber = 0;
    testNumber++;
    std::cout << (success ? GREEN2 : RED2) << DOT << RESET;
    if (!success)
        failedTests.push_back(testNumber);
}

int main(int argc, char **argv) {
    if (argc == 1)
        return (0);
    int index = std::atoi(argv[1]);
    switch (index)
    {
        case 1:
        {
            webserv::io::Socket socket(55555);
            try {
                webserv::io::Socket socket2(55555);
            } catch (std::exception &e) {
                return (1);
            }
            try {
                webserv::io::Socket socket3(55556);
                printVerdict(true);
            } catch (std::exception &e) {
                return (1);
            }
            return (0);
        }
        case 2:
        {
            try {
                webserv::io::Socket socket(2147483647);
                return (1);
            } catch (std::exception &e) {
                return (0);
            }
        }
        case 3:
        {
            try {
                webserv::io::Socket socket(0);
                return (1);
            } catch (std::exception &e) {
                return (0);
            }
        }
        case 4:
        {
            try {
                webserv::io::Socket socket(-1);
                return (1);
            } catch (std::exception &e) {
                return (0);
            }
        }
        default:
            return (2);
    }
}
