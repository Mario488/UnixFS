#include "RegularFile.h"
#include <iostream>
#include <fstream>

unsigned int RegularFile::uid = 0;
RegularFile::RegularFile(const std::string& name, FileType fileType): File(uid++, name, fileType){
    updateMetadataChangeTime();
}

size_t RegularFile::getFileSize() {return content.size();}

RegularFile::RegularFile(const RegularFile &other)
:File(uid++, other.getName(), FileType::RegularFile), content(other.content) {
    updateMetadataChangeTime();
}

const std::vector<char> &RegularFile::read(){
    updateAccessTime();
    return this->content;
}
std::string RegularFile::getName() const{
    return this->name;
}
const std::vector<char>& RegularFile::getContent(){
    updateAccessTime();
    return this->content;
}
// takes rvalue reference and moves the data
void RegularFile::write(std::vector<char>&& newData) {
    content = std::move(newData);
    size = content.size();
    updateModificationTime();
    updateMetadataChangeTime();
}
// to serialise the file
void RegularFile::serialize(std::ofstream &outFile){
    // Indicator 'F' for file
    char type = 'F';
    outFile.write(&type, sizeof(type));

    // Write id
    outFile.write(reinterpret_cast<char *>(&id), sizeof(id));

    // Write name length and name
    size_t nameLength = name.length();
    outFile.write(reinterpret_cast<char *>(&nameLength), sizeof(nameLength));
    outFile.write(name.c_str(), nameLength);

    // Write content size and content
    size_t contentSize = content.size();
    outFile.write(reinterpret_cast<char *>(&contentSize), sizeof(contentSize));
    outFile.write(content.data(), contentSize);

    // Write timestamps
    outFile.write(reinterpret_cast<char *>(&creationTime), sizeof(creationTime));
    outFile.write(reinterpret_cast<char *>(&lastModificationTime), sizeof(lastModificationTime));
    outFile.write(reinterpret_cast<char *>(&lastAccessTime), sizeof(lastAccessTime));
    outFile.write(reinterpret_cast<char *>(&lastMetadataChangeTime), sizeof(lastMetadataChangeTime));
}

// to deserialise the file
RegularFile *RegularFile::deserialize(std::ifstream &inFile)
{
    // Fetching the data in the same order we serialised it
    // First read the id
    uint32_t id;
    inFile.read(reinterpret_cast<char *>(&id), sizeof(id));

    // Second we read name length and name
    size_t nameLength;
    inFile.read(reinterpret_cast<char *>(&nameLength), sizeof(nameLength));

    std::string name;
    if (nameLength > 0)
    {
        name.resize(nameLength); // set expected length
        inFile.read(&name[0], nameLength); // fetching the name
    }
    else
    {
        throw std::invalid_argument("Name can't be empty");
    }

    // create RegularFile object
    RegularFile *file = new RegularFile(name, FileType::RegularFile);
    file->id = id; // set the id

    // read content size and content
    size_t contentSize;
    inFile.read(reinterpret_cast<char *>(&contentSize), sizeof(contentSize));
    if (contentSize > 0)
    {
        file->content.resize(contentSize);
        inFile.read(&file->content[0], contentSize);
    }

    // setting timestamps
    inFile.read(reinterpret_cast<char *>(&file->creationTime), sizeof(file->creationTime));
    inFile.read(reinterpret_cast<char *>(&file->lastModificationTime), sizeof(file->lastModificationTime));
    inFile.read(reinterpret_cast<char *>(&file->lastAccessTime), sizeof(file->lastAccessTime));
    inFile.read(reinterpret_cast<char *>(&file->lastMetadataChangeTime), sizeof(file->lastMetadataChangeTime));

    return file;
}

void RegularFile::append(std::vector<char> &&addData){
    content.insert(content.end(), 
    std::make_move_iterator(addData.begin()),
    std::make_move_iterator(addData.end()));
    size = content.size();
    updateModificationTime();
    updateMetadataChangeTime();
}
