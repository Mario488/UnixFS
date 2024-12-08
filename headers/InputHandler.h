#ifndef In_handler
#define In_handler
#include <iostream>
#include "FileSystem.h"
class InputHandler{
private:
    FileSystem* fs;
    std::string* const input;
public:
    InputHandler(std::string* const input, FileSystem* fs):input(input), fs(fs){}
    void inputHandler(bool& run);
};
#endif