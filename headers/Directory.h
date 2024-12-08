#include <vector>
#include "File.h"
#include "RegularFile.h"

class Directory: public File{
private:
    std::vector<RegularFile*> files;
    std::vector<Directory*> subDirs;
    std::string fullPath;
    Directory *parent;
public:
    static unsigned int uid;
    Directory(const std::string& name,FileType fType, Directory *parent);
    std::vector<RegularFile*>& getFiles();
    std::vector<Directory*>& getSubDirs();
    void addFile(const std::string &fileName);
    void addFile(RegularFile* file);
    void removeFile(RegularFile* file);
    void removeSubDir(Directory* dir);
    void addSubDir(const std::string &dirName);
    bool getFileByName(const std::string &fileName, RegularFile*& file);
    void serialize(std::ofstream &outFile);
    static Directory *deserialize(std::ifstream &inFile, Directory *parent);
    std::size_t getSize();
    Directory* getParent() const;
    std::string getName() const;
    std::string getPath() const;
};