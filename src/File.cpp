#include "File.h"

File::File(uint32_t id, const std::string& name, FileType fType){
    this->id = id;
    setName(name);
    this->fileType = fType;
    setCreationTime();
}
File::File()
{
    setCreationTime();
}
std::uint32_t File::getId() const { return this->id; }

const std::string &File::getName() const
{
    return this->name;
}
File::FileType File::getType() const {return this->fileType;}

time_t File::getLastAccessTime() const {return this->lastAccessTime;}

time_t File::getModificationTime() const {return this->lastModificationTime;}

time_t File::getMetadataChangeTime() const {return this->lastMetadataChangeTime;}

time_t File::getCreationTime() const{return this->creationTime;}

void File::updateAccessTime(){this->lastAccessTime = std::time(nullptr); }

void File::updateModificationTime(){
    this->lastAccessTime = std::time(nullptr); 
    updateMetadataChangeTime();
}

void File::updateMetadataChangeTime(){this->lastAccessTime = std::time(nullptr); }

void File::setCreationTime(){this->creationTime = std::time(nullptr);}

void File::setName(const std::string &name) {
    if(!name.empty()){
        this->name = name;
    }
    else{
        throw std::invalid_argument("Name cannot be empty");
    }
}
