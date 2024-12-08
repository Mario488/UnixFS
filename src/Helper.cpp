#include "Helper.h"
#include <sstream>
#include <string>
#include <iomanip>
#include <ctime>

bool Helper::startsWith(const std::string &str, const std::string &prefix){
    if(str.length() < prefix.length()) return false;
    return str.find(prefix) == 0;
}
std::vector<std::string> Helper::splitPath(const std::string &path) {
    std::stringstream ss(path);
    std::string component;
    std::vector<std::string> pathComponents;
    while (std::getline(ss, component, '/'))
    {
        if (!component.empty())
        {
            pathComponents.push_back(component);
        }
    }
    return pathComponents;
}
std::string Helper::getDirPathFromFilePath(const std::string &path){
    int lastSlash = path.find_last_of('/');
    if(lastSlash == std::string::npos){
        return "";
    }
    return path.substr(0, lastSlash);
}
bool Helper::parseDate(const std::string &dateStr, std::tm &date){
    if (dateStr.size() != 10 || dateStr[2] != '.' || dateStr[5] != '.'){
        return false; 
    }
    try{
        date.tm_mday = std::stoi(dateStr.substr(0, 2));        
        date.tm_mon = std::stoi(dateStr.substr(3, 2)) - 1;     
        date.tm_year = std::stoi(dateStr.substr(6, 4)) - 1900; 
    }
    catch (...){
        return false; 
    }
    if (date.tm_mday < 1 || date.tm_mday > 31 || date.tm_mon < 0 || date.tm_mon > 11){
        return false;
    }
    return true;
}
std::vector<std::string> Helper::split(const std::string &cmd){
    std::vector<std::string> parts;
    std::string temp = "";
    for(char c: cmd){
        if(c == ' '){
            parts.push_back(temp);
            temp = "";
            continue;
        }
        temp += c;
    }
    parts.push_back(temp);
    return parts;
}

bool Helper::includes(const std::string &str, char c){
    for(const char ch: str){
        if(ch == c){
            return true;
        }
    }
    return false;
}
