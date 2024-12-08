#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <ctime>
class File
{
public: 
enum class FileType
    {
        RegularFile,
        Directory,
        Symlink
    };
    uint32_t id;
    std::string name;
    size_t size;
    FileType fileType;
    time_t creationTime;
    time_t lastAccessTime;
    time_t lastModificationTime;
    time_t lastMetadataChangeTime;
    // Default constructor
    File(uint32_t id, const std::string& name, FileType fileType);
    File();
    virtual ~File() = default;
    // Getters and Setters
    std::uint32_t getId() const;
    const std::string& getName() const;
    FileType getType() const;
    time_t getLastAccessTime() const;
    time_t getModificationTime() const;
    time_t getMetadataChangeTime() const;
    time_t getCreationTime() const;
    void updateAccessTime();
    void updateModificationTime();
    void updateMetadataChangeTime();
    void setCreationTime();
private:
    void setName(const std::string& name);
};
#endif