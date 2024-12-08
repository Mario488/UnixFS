#ifndef REG_FILE_H
#define REG_FILE_H
#include "File.h"
#include <vector>
#include <utility>

class RegularFile: public File{
private:
    std::vector<char> content;
public:
    static unsigned int uid;
    RegularFile(const std::string& name, FileType fileType);
    size_t getFileSize();
    RegularFile(const RegularFile& other);
    const std::vector<char>& read();
    std::string getName() const;
    const std::vector<char>& getContent();
    void serialize(std::ofstream &outFile);
    static RegularFile *deserialize(std::ifstream &inFile);
    void write(std::vector<char>&& newData);
    void append(std::vector<char>&& addData);
};
#endif