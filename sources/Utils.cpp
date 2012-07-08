
#include <string>
#include <QDebug>
#include "Utils.h"

void Utils::replace(std::string &source, std::string oldValue, std::string newValue)
{// based on http://www.dreamincode.net/code/snippet5131.htm
    size_t x = source.find(oldValue, 0);
    while(x != std::string::npos) {
        source.erase(x, oldValue.length());
        source.insert(x, newValue);
        x += newValue.length();//prevents infinite recursion if something like ("sand", "and", "panda") is given
        if(x >= source.length()){
            return;
        }
        x = source.find(oldValue, x);
    }
}

std::string Utils::getFileName(const std::string &fullName)
{
    try {
        return fullName.substr(fullName.find_last_of("/") + 1);// +1 to avoid '/'
    } catch (std::exception e) {
        qCritical("@Utils::getFileName: exception!");
        qCritical() << e.what() ;
        return "";
    }
}

std::string Utils::getPath(const std::string &fullName)
{
    try{
        return fullName.substr(0, fullName.find_last_of("/"));
    } catch (std::exception e) {
        qCritical("@Utils::getFileName: exception!");
        qCritical() << e.what() ;
        return "";
    }
}
