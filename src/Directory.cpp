#include "Directory.h"
#include <vector>
#include <iostream>
#include <fstream>

unsigned int Directory::uid = 0;
Directory::Directory(const std::string& name,FileType fType, Directory *parent)
:File(uid++, name, fType), parent(parent){
    this->fullPath = getPath();
}

std::vector<RegularFile *> &Directory::getFiles(){
    return this->files;
}

std::vector<Directory *> &Directory::getSubDirs(){
    return this->subDirs;
}
// to serialize the directory
void Directory::serialize(std::ofstream &outFile){
    // Indicator 'D' for directory
    char type = 'D';
    outFile.write(&type, sizeof(type));

    // Write id
    outFile.write(reinterpret_cast<char *>(&id), sizeof(id));

    // Write name length and name
    size_t nameLength = name.length();
    outFile.write(reinterpret_cast<char *>(&nameLength), sizeof(nameLength));
    outFile.write(name.c_str(), nameLength);

    // serializing files
    size_t numFiles = files.size();
    outFile.write(reinterpret_cast<char *>(&numFiles), sizeof(numFiles));
    for (RegularFile *file : files)
    {
        file->serialize(outFile);
    }

    // serializing subdirectories
    size_t numSubDirs = subDirs.size();
    outFile.write(reinterpret_cast<char *>(&numSubDirs), sizeof(numSubDirs));
    for (Directory *dir : subDirs)
    {
        dir->serialize(outFile);
    }
}

// to deserialize the directory
Directory *Directory::deserialize(std::ifstream &inFile, Directory *parent)
{
    // Fetching the data in the same order we serialized it
    // First read the id
    uint32_t id;
    inFile.read(reinterpret_cast<char *>(&id), sizeof(id));

    // Second we read name length and name
    size_t nameLength;
    inFile.read(reinterpret_cast<char *>(&nameLength), sizeof(nameLength));

    std::string name;
    if (nameLength > 0)
    {
        name.resize(nameLength);
        inFile.read(&name[0], nameLength);
    }
    else
    {
        name = "/";
    }

    // create Directory object
    Directory *dir = new Directory(name, FileType::Directory, parent);
    dir->id = id; // Set the correct ID

    size_t numFiles;
    inFile.read(reinterpret_cast<char *>(&numFiles), sizeof(numFiles));
    for (size_t i = 0; i < numFiles; ++i)
    {
        // get type indicator
        char type;
        inFile.read(&type, sizeof(type));
        if (type == 'F')
        {
            RegularFile *file = RegularFile::deserialize(inFile);
            dir->files.push_back(file);
        }
        else
        {
            throw std::runtime_error("RegularFile expected");
        }
    }

    // deserialize subdirectories
    size_t numSubDirs;
    inFile.read(reinterpret_cast<char *>(&numSubDirs), sizeof(numSubDirs));
    for (size_t i = 0; i < numSubDirs; ++i)
    {
        // read type indicator
        char type;
        inFile.read(&type, sizeof(type));
        if (type == 'D')
        {
            Directory *subDir = Directory::deserialize(inFile, dir);
            dir->subDirs.push_back(subDir);
        }
        else
        {
            throw std::runtime_error("Directory expected");
        }
    }

    return dir;
}

void Directory::addFile(const std::string &fileName){
    for (RegularFile *file : getFiles()){
        if (file->getName() == fileName){
            std::cout << "File with the same name exists in this directory" << std::endl;
            return;
        }
    }
    RegularFile *file1 = new RegularFile(fileName, FileType::RegularFile);
    files.push_back(file1);
}

void Directory::addFile(RegularFile *file){
    for (RegularFile *f : getFiles()){
        if (f->getName() == file->getName()){
            std::cout << "File with the same name exists in this directory" << std::endl;
            return;
        }
    }
    files.push_back(file);
}

void Directory::removeFile(RegularFile *file){
    std::vector<RegularFile*> &files = getFiles();
    for(int i = 0; i < files.size(); i++){
        if(files[i] == file){
            files.erase(files.begin() + i);
            return;
        }
    }
}
void Directory::removeSubDir(Directory *dir){
    std::vector<Directory*> &dirs = getSubDirs();
    for(int i = 0; i < dirs.size(); i++){
        if(dirs[i] == dir){
            dirs.erase(dirs.begin() + i);
            return;
        }
    }
}

void Directory::addSubDir(const std::string &dirName){
    for(Directory* dir: getSubDirs()){
        if(dir->getName() == dirName){
            std::cout << "Directory with the same name exists in this directory" << std::endl;
            return;
        }
    }
    Directory* newDir = new Directory(dirName, FileType::Directory, this);
    subDirs.push_back(newDir);
}
bool Directory::getFileByName(const std::string &fileName, RegularFile*& file){
    for(RegularFile* f: getFiles()){
        if(f->getName() == fileName){
            file = f;
            return true;
        }
    }
    return false;
}

Directory *Directory::getParent() const{
    return this->parent;
}

std::string Directory::getPath() const{
    if(!parent){
        return name; // root directory
    }
    else{
        std::string parentPath = parent->getPath();
        if(parentPath == "/"){
            return parentPath + name;
        }
        else{
            return parentPath + "/" + name;
        }
    }
}
std::size_t Directory::getSize() {
    size_t res = 0;
    for(RegularFile* file: getFiles()){
        res += file->getFileSize();
    }
    return res;
}
std::string Directory::getName() const{
    return this->name;
}
