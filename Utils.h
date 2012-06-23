#ifndef UTILS_H
#define UTILS_H

#include <string>

class Utils
{
    public:
        static void replace(std::string &source, std::string oldValue, std::string newValue);
        static std::string getFileName(const std::string &fullName);
        static std::string getPath(const std::string &fullName);
};

#endif // UTILS_H
