#include <iostream>
#include <string>
#include <fstream>
#include "FileSystem.h"
#include "InputHandler.h"

int main(int argc, char *argv[])
{
    std::string filePath;

    // Check if command line argument is provided
    if (argc > 1){
        filePath = argv[1];
    }
    else{
        std::cout << "Enter the path to the file containing the file system: ";
        std::getline(std::cin, filePath);
    }

    // Initialise the file system
    FileSystem *fs = new FileSystem();

    // Try loading the file system
    std::ifstream inFile(filePath, std::ios::binary);
    // Check if file exists or can be opened
    if (inFile.good()){
        fs->loadFromFile(filePath);
        std::cout << "File system loaded from " << filePath << std::endl;
    }
    else{
        std::cout << "File system doesn't exist. New one will be created" << std::endl;
    }

    inFile.close();

    // Initialize InputHandler
    std::string input;
    InputHandler inHandler(&input, fs);

    bool run = true;
    while (run)
    {
        std::cout << "mario-unix: ";
        std::getline(std::cin, input);
        inHandler.inputHandler(run);
    }

    // save the file system before exiting
    fs->saveToFile(filePath);
    std::cout << "File system saved to " << filePath << std::endl;

    delete fs;

    return 0;
}
