#include "InputHandler.h"
#include "Helper.h"
#include <string>
#include <vector>

void InputHandler::inputHandler(bool& run){
    const std::string& userInput = *input;
    if(userInput == "pwd"){
        std::cout << fs->getCurrDirPath() << std::endl;
        return;
    }
    if(userInput == "exit"){
        run = false;
        return;
    }
    if(Helper::startsWith(userInput, "cd")){
        // split the user input by space to get the arguments
        std::vector<std::string> parts = Helper::split(userInput);
        if(parts.size() >= 2){
            std::string path = parts[1];
            fs->changeDirectory(path);
            return;
        }
        else{
            std::cout << "Usage: cd <path>" << std::endl;
            return;
        }
    }
    if(Helper::startsWith(userInput, "ls")){
        std::vector<std::string> parts = Helper::split(userInput);
        std::string res = "";
        if(parts.size() == 1){
            res = fs->listFiles("");
        }
        else{
            res = fs->listFiles(parts[1]);
        }
        if(!res.empty()){
            std::cout << res << std::endl;
        }
        else{
            std::cout << "Empty directory" << std::endl;
        }
        return;
    }
    if(Helper::startsWith(userInput, "mkdir")){
        std::vector<std::string> parts = Helper::split(userInput);
        if(parts.size() >= 2){
            std::vector<std::string> dirs (parts.begin() + 1, parts.end());
            fs->makeDir(dirs);
        }
        else{
            std::cout << "Usage: mkdir <directory>" << std::endl;
        }
        return;
    }
    if(Helper::startsWith(userInput, "cat")){
        std::vector<std::string> parts = Helper::split(userInput);
        std::string outputFileName;
        if(Helper::includes(userInput, '>')){
            // the last given argument is the output file path
            outputFileName = parts.back();
            // get only file paths
            std::vector<std::string> inputFiles (parts.begin() + 1, parts.end() - 2);
            fs->cat(inputFiles, outputFileName);
        }
        else{
            outputFileName = "";
            std::vector<std::string> inputFiles (parts.begin() + 1, parts.end());
            fs->cat(inputFiles, outputFileName);
        } 
        return;
    }
    if(Helper::startsWith(userInput, "cp")){
        std::vector<std::string> parts = Helper::split(userInput);
        if(parts.size() >= 3){
            std::string outputDir = parts.back();
            std::vector<std::string> files (parts.begin() + 1, parts.end() - 1);
            fs->cp(files, outputDir);
        }
        else{
            std::cout << "Usage: cp [<file>] <dir>" << std::endl;
        }
        return;
    }
    if(Helper::startsWith(userInput, "stat")){
        std::vector<std::string> parts = Helper::split(userInput);
        if(parts.size() >= 2){
            std::vector<std::string> files (parts.begin() + 1, parts.end());
            fs->stat(files);
        }
        else{
            std::cout << "Usage: stat [<file>]" << std::endl;
        }
        return;
    }
    if(Helper::startsWith(userInput, "locate")){
        std::vector<std::string> parts = Helper::split(userInput);
        if(parts.size() == 4){
            std::vector<std::string> filter (parts.begin() + 1, parts.end());
            fs->locate(filter, "");
        }
        else if(parts.size() == 5){
            std::vector<std::string> filter (parts.begin() + 1, parts.end() - 1);
            std::string dirPath = parts.back();
            fs->locate(filter, dirPath);
        }
        else{
            std::cout << "Usage: locate <filter> [<directory>]" << std::endl;
        }
        return;
    }
    if(Helper::startsWith(userInput, "import")){
        std::vector<std::string> parts = Helper::split(userInput);
        if(parts.size() == 2){
            fs->import(parts.back(), "");
        }
        else if(parts.size() == 3){
            fs->import(parts[1], parts[2]);
        }
        else{
            std::cout << "Usage: import <file> [<directory>]" << std::endl;
        }
        return;
    }
    if(Helper::startsWith(userInput, "rmdir")){
        std::vector<std::string> parts = Helper::split(userInput);
        if(parts.size() >= 2){
            std::vector<std::string> dirs (parts.begin() + 1, parts.end());
            fs->rmdir(dirs);
        }
        else{
            std::cout << "Usage: rmdir [<dir>]" << std::endl;
        }
        return;
    }
    if(Helper::startsWith(userInput, "rm")){
        std::vector<std::string> parts = Helper::split(userInput);
        if(parts.size() >= 2){
            std::vector<std::string> files (parts.begin() + 1, parts.end());
            fs->rm(files);
        }
        else{
            std::cout << "Usage: rm [<file>]" << std::endl;
        }
        return;
    }
    std::cout << "Invalid command" << std::endl;
}

