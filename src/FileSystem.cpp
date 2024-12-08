#include "FileSystem.h"
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <utility>
#include <fstream>
#include <ctime>
#include "Helper.h" 

FileSystem::FileSystem(){
    // Initialize the root and current working directory
    this->root = new Directory("/",File::FileType::Directory, nullptr);
    this->currDir = root;
}
// Clean up the root
FileSystem::~FileSystem(){
    delete root;
}

void FileSystem::makeDir(const std::vector<std::string>& dirs){
    for(const std::string& dirPath: dirs){
        // here just serves as a way to remove the last segment dir
        std::string pathToParent = Helper::getDirPathFromFilePath(dirPath); 
        Directory* parentDir = findDir(pathToParent);
        if(!parentDir){
            std::cout << "Invalid directory path" << std::endl;
            continue; // skip to next directory
        }
        // find the index of last slash to get the dir name
        int lastSlash = dirPath.find_last_of('/');
        // extract ony the directory name
        std::string newDirName = (lastSlash != std::string::npos) ? dirPath.substr(lastSlash+1): dirPath;
        parentDir->addSubDir(newDirName);
    }
}

// get file name and find that file
void FileSystem::saveToFile(const std::string &filename){
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile)
    {
        std::cerr << "Cannot open file for writing: " << filename << std::endl;
        return;
    }

    // save the id's
    outFile.write(reinterpret_cast<char *>(&RegularFile::uid), sizeof(RegularFile::uid));
    outFile.write(reinterpret_cast<char *>(&Directory::uid), sizeof(Directory::uid));

    // serialize the root dir
    root->serialize(outFile);

    outFile.close();
}

void FileSystem::loadFromFile(const std::string &filename){
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile){
        std::cout << "Can't open file: " << filename << std::endl;
        return;
    }
    try{
        // loading the id's
        inFile.read(reinterpret_cast<char *>(&RegularFile::uid), sizeof(RegularFile::uid));
        inFile.read(reinterpret_cast<char *>(&Directory::uid), sizeof(Directory::uid));
        // read the types 
        char type;
        inFile.read(&type, sizeof(type));
        if (type != 'D'){
            throw std::runtime_error("Invalid type");
        }
        // Deserialize the root directory
        if (root){
            delete root;
        }
        root = Directory::deserialize(inFile, nullptr);
        currDir = root;
    }
    catch (const std::exception &ex){
        std::cerr << "An error occured " << ex.what() << std::endl;
        if (root){
            delete root;
            root = nullptr;
        }
        currDir = nullptr;
    }
    inFile.close();
}

std::vector<char> FileSystem::gatherInputFromStdin(){
    std::vector<char> inputVec;
    std::string line;
    while(true){
        std::getline(std::cin, line);
        if(line == ".") { break; }
        inputVec.insert(inputVec.end(), line.begin(), line.end());
        inputVec.push_back('\n');
    }
    return inputVec;
}

void FileSystem::cp(const std::vector<std::string> &files, const std::string &outputDir){ 
    Directory* outDir = findDir(outputDir);
    if(!outDir) {
        std::cout << "Invalid output directory" << std::endl;
        return;
    }
    // find the files and if they exist copy them into out outputDir
    std::vector<RegularFile*> filePtrs;
    for(const std::string& filePath: files){

        RegularFile* currFile = findFile(filePath,false);
        // check if it couldn't create file
        if(!currFile){
            std::cout << "Invalid file path" << std::endl;
            return;
        }
        filePtrs.push_back(currFile);
    }
    for(RegularFile* filePtr: filePtrs){
        RegularFile* newFile = new RegularFile(*filePtr);
        outDir->addFile(newFile);
    }
}

std::string FileSystem::getCurrDirPath() const{
    return currDir->getPath();
}

void FileSystem::changeDirectory(const std::string &path){
    if (path == ".") { return; }
    // check for absolute or relative path
    Directory *newDir = (path[0] == '/') ? root : currDir;
    std::vector<std::string> pathComponents = Helper::splitPath(path);
    for (const std::string &comp : pathComponents){
        if (comp == "."){
            continue;
        }
        else if (comp == ".."){
            if (newDir->getParent()){
                newDir = newDir->getParent();
            }
            else{
                std::cout << "Already at root directory." << std::endl;
            }
        }
        else{
            bool found = false;
            for (Directory *subDir : newDir->getSubDirs()){
                if (subDir->getName() == comp){
                    newDir = subDir;
                    found = true;
                    break;
                }
            }
            if (!found){
                std::cout << "Directory not found: " << comp << std::endl;
                return;
            }
        }
    }
    currDir = newDir;
}
std::string FileSystem::printDir(Directory& dir){

    std::string res = "";
    for (Directory *cDir : dir.getSubDirs()){
        res += cDir->getName() + " ";
    }
    for (RegularFile *RegularFile : dir.getFiles()){
        res += RegularFile->getName() + ".txt ";
    }
    if (res.empty()){
        return "This directory is empty";
    }
    return res;
}

Directory *FileSystem::findDir(const std::string &path){
    Directory *tempDir = (path[0] == '/') ? root: currDir;
    std::vector<std::string> pathcomps = Helper::splitPath(path);
    for(int i = 0; i < pathcomps.size(); i++){
        bool found = false;
        for(Directory* dir: tempDir->getSubDirs()){
            if(dir->getName() == pathcomps[i]){
                tempDir = dir;
                found = true;
                break;
            }
        }
        if(!found) {return nullptr;}
    }
    return tempDir;
}
std::vector<char> FileSystem::gatherInputContent(const std::vector<std::string> &inputFiles, bool& state){
    std::vector<char> inputVec;
    for(const std::string& inFilePath: inputFiles){
        RegularFile* currFile = findFile(inFilePath, false);
        if(!currFile){
            state = false;
            break;
        }
        const std::vector<char>& fileContent = currFile->getContent();
        inputVec.insert(inputVec.end(), fileContent.begin(), fileContent.end());
    }
    return inputVec;
}
void FileSystem::cat(const std::vector<std::string> &inputFiles, const std::string &outputFile){
    // check if outputfile is not provided
    if(outputFile.empty()){
        bool state = true;
        std::vector<char> inputVec = gatherInputContent(inputFiles, state);
        if(!state){
            std::cout << "Incorrect file path" << std::endl;
            return;
        }
        for(const char c: inputVec){
            std::cout << c;
        }
        return;
    }
    // flag - true, to create the file if it doesn't exist already and can be created
    RegularFile* outFile = findFile(outputFile,true);
    // incorrect file path
    if(!outFile){
        std::cout << "Invalid output file path" << std::endl;
        return;
    }
    // if no files are provided get input from std in
    if(inputFiles.empty()){
        std::vector<char> stdIn = gatherInputFromStdin();
        outFile->write(std::move(stdIn));
        return;
    }
    bool state = true;
    std::vector<char> inputVec = gatherInputContent(inputFiles, state);
    if(!state){
        std::cout << "Incorrect file path" << std::endl;
        return;
    }
    outFile->write(std::move(inputVec));
}
RegularFile *FileSystem::findFile(Directory *const dir, const std::string& fileName)
{
    for(RegularFile* file: dir->getFiles()){
        if(file->getName() == fileName){
            // we have found the file
            return file;
        }
    }
    // file doesn't exist
    return nullptr;
}

void FileSystem::rmdir(const std::vector<std::string> &dirs)
{
    for(const std::string& dirPath: dirs){
        Directory* dir = findDir(dirPath);
        if(!dir){
            std::cout << "Couldn't find directory" << std::endl;
            continue;
        }
        if(!dir->getFiles().empty() || !dir->getSubDirs().empty()){
            std::cout << "Directory is not empty" << std::endl;
            continue;
        }
        Directory* parentDir = dir->getParent();
        if(!parentDir){
            std::cout << "Couldn't find parent directory" << std::endl;
            continue;
        }
        parentDir->removeSubDir(dir);
        delete dir;
    }
}
void FileSystem::stat(const std::vector<std::string> &files){
    for(const std::string& filePath: files){
        RegularFile* file = findFile(filePath, false);
        if(!file){
            std::cout << "Couldn't find file" << std::endl;
            continue;
        }
        std::cout << "Id: " << file->getId() << std::endl;
        std::cout << "Size: " << file->getFileSize() << std::endl;
        std::cout << "Last Access: " << file->getLastAccessTime() << std::endl;
        std::cout << "Last Modification: " << file->getModificationTime() << std::endl;
        std::cout << "Last Metadata change: " << file->getMetadataChangeTime() << std::endl;
    }
}
void FileSystem::rm(const std::vector<std::string> &files)
{
    // find the files and delete them
    for(const std::string& filePath: files){
        RegularFile* file = findFile(filePath, false);
        if(!file){
            std::cout << "Couldn't find file" << std::endl;
            continue;
        }
        std::string dirPath = Helper::getDirPathFromFilePath(filePath);
        Directory* fileDir = findDir(dirPath);
        if(!fileDir){
            std::cout << "Directory not found" << std::endl;
            continue;
        }
        fileDir->removeFile(file);
        delete file;
    }
}

void FileSystem::import(const std::string &filePath, const std::string& dirPath){
    // find the positions of the last \ and . in order to extract only the file name without the extension  
    int pos1 = filePath.find_last_of("\\"); 
    int pos2 = filePath.find_last_of(".");
    std::string fileName = (pos1 == std::string::npos) ? filePath : filePath.substr(pos1 + 1, pos2 - (pos1 + 1));
    std::ifstream file(filePath);
    if(!file.good()) {
        std::cout << "File does not exists" << std::endl;
        return;
    }
    if(!file.is_open()){
        std::cout << "Failed to open file" << std::endl;
        return;
    }
    // create a buffer and read the file into it
    std::stringstream buf;
    buf << file.rdbuf();
    std::string bufStr = buf.str();
    RegularFile* newFile = new RegularFile(fileName, FileType::RegularFile);
    // fill the char vector with the text
    std::vector<char> content (bufStr.begin(), bufStr.end());
    newFile->write(std::move(content));
    if(dirPath.empty()){
        currDir->addFile(newFile);
        return;
    }
    Directory* dir = findDir(dirPath);
    if(!dir){
        std::cout << "Invalid directory path" << std::endl;
        return;
    }
    dir->addFile(newFile);
}

void FileSystem::locate(const std::vector<std::string> &filter, const std::string &dir){
    // divide the filter arguments
    std::string type = filter[0];
    std::string relation = filter[1];
    std::string value = filter[2];
    Directory* temp = dir.empty() ? currDir : findDir(dir);
    if(!temp){
        std::cout << "Directory doesn't exists" << std::endl;
        return;
    }
    // recursively call all sub directories 
    for(Directory* subDir: temp->getSubDirs()){
        locate(filter, subDir->getName());
    }
    for(RegularFile* file: temp->getFiles()){
        bool match = false;
        if(type == "name" && relation == "has"){
            if(file->getName().find(value) != std::string::npos){
                match = true;
            }
        }
        else if(type == "size"){
            size_t fileSize = file->getFileSize();
            size_t filterSize = static_cast<size_t>(std::stoull(value));
            if((relation == "<" && fileSize < filterSize) ||
            (relation == ">" && fileSize > filterSize) ||
            (relation == "=" && fileSize == filterSize)){
                match = true;
            }
        }
        else if(type == "content" && relation == "has"){
            std::string content (file->getContent().data(), file->getContent().size());
            if(content.find(value) != std::string::npos){
                match = true;
            }
        }
        else if(type == "crdate" || type == "mdate"){
            time_t fileTimeT = (type == "crdate") ? file->getCreationTime() : getModificationTime();

            std::tm filterTime = {};
            if(!Helper::parseDate(value, filterTime)){
                std::cout << "Invalid date format" << std::endl;
                return;
            }
            std::time_t filterTimeT = std::mktime(&filterTime);
            if(filterTimeT == -1){
                std::cout << "Erorr converting time" << std::endl;
                return;
            }
            if(relation == "<"){
                if(fileTimeT < filterTimeT){
                    match = true;
                }
            }
            else if(relation == "="){
                if(fileTimeT == filterTimeT){
                    match = true;
                }
            }
            else if(relation == ">"){
                if(fileTimeT > filterTimeT){
                    match = true;
                }
            }
        }
        if(match){
            std::cout << "Id: " << file->getId() << std::endl;
            std::cout << "Name: " << file->getName() << std::endl;
            std::cout << "Size: " << file->getFileSize() << std::endl;
            std::cout << "Last Access: " << file->getLastAccessTime() << std::endl;
            std::cout << "Last Modification: " << file->getModificationTime() << std::endl;
            std::cout << "Last Metadata change: " << file->getMetadataChangeTime() << std::endl;
            std::cout << "-------------------------" << "\n";
        }
    }
}

RegularFile *FileSystem::findFile(const std::string &path, bool createFlag){
    if(path.empty()) {return nullptr;}
    std::vector<std::string> parts = Helper::splitPath(path);
    RegularFile* file = nullptr;
    RegularFile* newFile = nullptr; // initializing in case the flag is true
    const std::string& fileName = parts.back(); // assuming the last segment of the path is the file name
    // means only the file name is provided and we search the current directory 
    if(parts.size() == 1){
        file = findFile(currDir, fileName);
        if(!file && createFlag) {
            newFile = new RegularFile(fileName, FileType::RegularFile);
            currDir->addFile(newFile);
            return newFile;
        }
        return file;
    }
    std::string dirPath = Helper::getDirPathFromFilePath(path);
    Directory* fileDir = findDir(dirPath); // first search for the directory
    if(!fileDir){ return nullptr; } // we haven't found the directory
    file = findFile(fileDir, fileName); 
    if(!file && createFlag){ // if we havent't found the file but we want it created
        newFile = new RegularFile(fileName, FileType::RegularFile);
        fileDir->addFile(newFile);
        return newFile;
    }
    return file;
}

std::string FileSystem::listFiles(const std::string &path) 
{
    if(path.empty()){
        return printDir(*currDir);
    }
    // checks whether the path is relative or absolute
    Directory *tempDir = findDir(path);
    if(!tempDir) { return "Invalid file path"; }
    return printDir(*tempDir);
}