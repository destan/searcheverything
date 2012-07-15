/*
    AUTHOR:
    Destan Sarpkaya - 2012
    destan@dorukdestan.com

    LICENSE:
    SearchEverything is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SearchEverything is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SearchEverything.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <QDebug>
#include <QStyle>
#include <QDesktopWidget>
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

void Utils::centerWindow(QWidget *window) {
    QRect frect = window->frameGeometry();
    frect.moveCenter(QDesktopWidget().availableGeometry().center());
    window->move(frect.topLeft());
}
