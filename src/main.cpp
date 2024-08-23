#include "createcpp.h"
#include <iostream>
#include <qt5/QtCore/QString>
#include <qt5/QtCore/QStringList>
#include <filesystem>
#include "creategen.h"

namespace fs = std::filesystem;

std::string VERSION = "CreateC++ v1.0 pre-release 3";

int generateMinimalConfigFile(){
    std::string project, root;
    int cppstd = 17;

    std::cout << "Enter project name\n:: ";
    std::cin >> project;
    std::cout << "Enter project root (leave empty for current dir)\n:: ";
    std::getline(std::cin, root);

    if (root == "") root = "./";
    if (!QString::fromStdString(root).endsWith("/")) root += "/";

    std::cout << "Enter C++ standard\n:: ";
    std::cin >> cppstd;

    if (cppstd != 17 && cppstd != 14 && cppstd != 11 && cppstd != 20 && cppstd != 23 && cppstd != 98){
        logError("Invalid c++ standard!");
        return -1;
    }

    generateMinimalCreateCPPConfiguration(project, root, cppstd);
    return 0;
}

int main(int argc, char const *argv[])
{
    QStringList genConfigTypes;
    genConfigTypes.push_back("minimal");

    if (argc == 1)
    {
        std::cerr << "No argument provided" << std::endl;
        return -1;
    }

    QString arg(argv[1]);

    // if the user is checking the version
    if (arg == "-v" || arg == "-V" || arg == "--version" || arg == "-version" || arg == "--v")
    {
        std::cout << VERSION << std::endl;
        return 0;
    }

    // If the user is a lazy ahh guy and wants to generate the config file
    if (arg == "--generate" || arg == "-g"){
        if (argc == 2){
            std::cerr << "No config file type provided.\n";
            std::cerr << "Examples:\n";
            for (const auto& t : genConfigTypes){
                std::cout << "- " << t.toStdString() << "\n";
            }
            return -1;
        }
        
        QString genType(QString::fromStdString(argv[2]));
        if (!genConfigTypes.contains(genType)){
            std::cerr << "Invalid config type.\nHere are some examples:\n";
            for (const auto& t : genConfigTypes){
                std::cout << "- " << t.toStdString() << "\n";
            }
            return -1;
        }

        if (genType == genConfigTypes.at(0)){
            return generateMinimalConfigFile();
        }
    }

    // if the user is trying to build a project

    if (arg == "-b")
    {
        if (argc == 2)
        {
            std::cerr << "No input file is provided.\n";
            return -1;
        }
        auto buildFile = argv[2];

        if (buildFile == ".." || buildFile == "." || fs::is_directory(buildFile))
        {
            return build(QString("%1/create.json").arg(buildFile).toStdString().c_str());
        }

        return build(buildFile);
    }

    if (arg == "-i")
    {
        if (!fs::exists("./createc_cache.json"))
        {
            logError("Could not install executable onto the system. No cache file is found.");
            return -1;
        }
        json cacheFile = loadCacheFile();
        std::system(QString("sudo cp ./%1 /usr/bin").arg(QString::fromStdString(cacheFile["executable"])).toStdString().c_str());
    }

    if (arg == "-u")
    {
        if (!fs::exists("./createc_cache.json"))
        {
            logError("Could not install executable onto the system. No cache file is found.");
            return -1;
        }
        json cacheFile = loadCacheFile();
        std::system(QString("sudo rm -rf /usr/bin/%1").arg(QString::fromStdString(cacheFile["executable"])).toStdString().c_str());
    }

    return 0;
}
