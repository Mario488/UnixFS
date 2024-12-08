#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "Directory.h"
#include "File.h"
#include "RegularFile.h"
#include <vector>
#include <iostream>

class FileSystem : public File
{
private:
    Directory *root;
    Directory *currDir;
    std::string printDir(Directory& dir);
    Directory* findDir(const std::string& path);
    // flag - true (create the file if it's possible)
    RegularFile* findFile(const std::string& path, bool createFlag);
    RegularFile* findFile(Directory* const dir, const std::string& fileName);
    std::vector<char> gatherInputContent(const std::vector<std::string>& inputFiles, bool& state);
    std::vector<char> gatherInputFromStdin();
public:
    FileSystem();
    ~FileSystem(); 
    void saveToFile(const std::string &filename);
    void loadFromFile(const std::string &filename);
    std::string getCurrDirPath() const;
    // All of the commands
    void changeDirectory(const std::string &path);
    void makeDir(const std::vector<std::string>& dirs);
    void cat(const std::vector<std::string>& inputFiles, const std::string& outputFile);
    void cp(const std::vector<std::string>& files, const std::string& outputDir);
    void rm(const std::vector<std::string>& files);
    void import(const std::string& file, const std::string& dir);
    void locate(const std::vector<std::string>& filter, const std::string& dir);
    void rmdir(const std::vector<std::string>& dirs);
    void stat(const std::vector<std::string>& files);
    std::string listFiles(const std::string &path);
};
#endif