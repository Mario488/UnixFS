#ifndef Helper_h
#define Helper_h
#include <vector>
#include <string>
#include <ctime>

namespace Helper{
    bool startsWith(const std::string &str,
    const std::string &prefix);
    std::vector<std::string> split(const std::string& cmd);
    bool includes(const std::string &str, char c);
    std::vector<std::string> splitPath(const std::string &path);
    std::string getDirPathFromFilePath(const std::string& path);
    bool parseDate(const std::string &dateStr, std::tm &date);
}
#endif