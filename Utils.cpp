
#include <string>

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
