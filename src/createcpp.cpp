#include <json.hpp>
#include <qt5/QtCore/QString>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "createcpp.h"

namespace fs = std::filesystem;
using json = nlohmann::json;

std::string infoColor = "\033[34m";
std::string errorColor = "\033[31m";
std::string warnColor = "\033[33m";

void logInfo(std::string text)
{
    std::cout << infoColor << "[INF] \033[0m" << text << std::endl;
}

void logWarning(std::string text)
{
    std::cout << warnColor << "[WARN] \033[0m" << text << std::endl;
}

void logError(std::string text)
{
    std::cout << errorColor << "[ERR] \033[0m" << text << std::endl;
}


void doLibraryThing(QString *command, std::string lib)
{
    if (lib == "gtkmm")
    {
        if (!isPackageInstalled(lib))
        {
            askToInstallPackage(lib);
        }

        QString text("`pkg-config --cflags --libs gtkmm-3.0`");
        text.append(" ");
        command->append(text);
        return;
    }

    else if (lib == "gtk3")
    {
        if (!isPackageInstalled(lib))
        {
            askToInstallPackage(lib);
        }

        QString text("`pkg-config --cflags --libs gtk+-3.0` ");
        command->append(text);
        return;
    }

    else if (lib == "gtk4")
    {
        if (!isPackageInstalled(lib))
        {
            askToInstallPackage(lib);
        }

        QString text("`pkg-config --cflags --libs gtk+-4.0` ");
        command->append(text);
        return;
    }

    else if (lib == "qt5widgets")
    {
        if (!isPackageInstalled(lib))
        {
            askToInstallPackage(lib);
        }

        QString text("`pkg-config --cflags --libs Qt5Widgets` -fPIC ");
        command->append(text);
        return;
    }

    else if (lib == "qt5core")
    {
        if (!isPackageInstalled(lib))
        {
            askToInstallPackage(lib);
        }

        QString text("`pkg-config --cflags --libs Qt5Core` -fPIC ");
        command->append(text);
        return;
    }

    else if (lib == "qt5gui")
    {
        if (!isPackageInstalled(lib))
        {
            askToInstallPackage(lib);
        }

        QString text("`pkg-config --cflags --libs Qt5Gui` -fPIC ");
        command->append(text);
        return;
    }

    QString text("-l");
    text.append(QString::fromStdString(lib));
    text.append(" ");
    command->append(text);
}


bool isPackageInstalled(std::string package)
{
    if (package == "gtk3")
    {
        if (std::system("pkg-config --exists gtk+-3.0") == 0)
            return true;
        else
            return false;
    }

    else if (package == "gtkmm")
    {
        if (std::system("pkg-config --exists gtkmm-3.0") == 0)
            return true;
        else
            return false;
    }

    else if (package == "qt5gui")
    {
        if (std::system("pkg-config --exists Qt5Gui") == 0)
            return true;
        else
            return false;
    }

    else if (package == "qt5core")
    {
        if (std::system("pkg-config --exists Qt5Core") == 0)
            return true;
        else
            return false;
    }

    else if (package == "qt5widgets")
    {
        if (std::system("pkg-config --exists Qt5Widgets") == 0)
            return true;
        else
            return false;
    }

    return false;
}

void createCacheFile(QString projectName, QString executable)
{
    json cacheData;

    cacheData["project"] = projectName.toStdString();
    cacheData["executable"] = executable.toStdString();

    std::ofstream file("createc_cache.json");
    if (file.is_open())
    {
        file << cacheData.dump(4);
        file.close();
        return;
    }
    else
    {
        logError("Could not create cache file.");
    }
}

json loadCacheFile()
{
    json cacheFile;

    std::ifstream file("createc_cache.json");
    if (file.is_open())
    {
        file >> cacheFile;
        file.close();
        return cacheFile;
    }
    else
    {
        logError("Could not load cache file.");
        return json();
    }
}

void runScripts(auto scripts)
{
    for (const auto &script : scripts)
    {
        int res = std::system(QString("bash %1").arg(QString::fromStdString(script)).toStdString().c_str());
        if (res != 0)
        {
            logError(QString("Error in script: %1").arg(QString::fromStdString(script)).toStdString().c_str());
            std::exit(-1);
        }
    }
}

void installPackage(std::string package)
{
    if (package == "gtk3")
    {
        std::system("sudo apt install libgtk-3-dev");
    }

    else if (package == "gtk4")
    {
        std::system("sudo apt install libgtk-4-dev");
    }

    else if (package == "gtkmm")
    {
        std::system("sudo apt install libgtkmm-3.0-dev");
    }

    else if (package == "qt5gui" || package == "qt5core" || package == "qt5widgets")
    {
        std::system("sudo apt install qtbase5-dev");
    }
    else if (package == "SDL2")
    {
        std::system("sudo apt install libsdl2-dev");
    }
}

void askToInstallPackage(std::string package)
{
    logWarning("The following package is needed but isn't installed: " + package);
    logWarning("Would you like to install it? (skip if you dont have 'apt')");

    logWarning("Y/n");

    char choice;
    std::cin >> choice;

    if (choice != 'y' && choice != 'Y' && choice != 'n' && choice != 'N')
    {
        logError("invalid choice.");
        std::exit(-1);
    }

    if (choice == 'y' || choice == 'Y')
    {
        installPackage(package);
    }

    else
    {
        return;
    }
}

std::vector<std::string> scanForSourceFilesInDir(std::string dir)
{
    std::vector<std::string> files;
    for (auto file : fs::directory_iterator(dir))
    {
        if (!file.is_regular_file())
            continue;

        QString qFile = QString::fromStdString(file.path());
        if (!qFile.endsWith(".c") && !qFile.endsWith(".cpp"))
            continue;

        files.push_back(file.path());
    }
    return files;
}

std::vector<std::string> scanForSourceFiles(std::string root)
{
    std::vector<std::string> files;
    if (fs::exists(root + "src"))
    {
        std::vector<std::string> newFiles = scanForSourceFilesInDir(root + "src");
        for (std::string newFile : newFiles)
            files.push_back(newFile);
    }
    if (fs::exists(root + "sources"))
    {
        std::vector<std::string> newFiles = scanForSourceFilesInDir(root + "sources");
        for (std::string newFile : newFiles)
            files.push_back(newFile);
    }
    if (fs::exists(root + "include"))
    {
        std::vector<std::string> newFiles = scanForSourceFilesInDir(root + "include");
        for (std::string newFile : newFiles)
            files.push_back(newFile);
    }
    if (fs::exists(root + "lib"))
    {
        std::vector<std::string> newFiles = scanForSourceFilesInDir(root + "lib");
        for (std::string newFile : newFiles)
            files.push_back(newFile);
    }
    if (fs::exists(root))
    {
        std::vector<std::string> newFiles = scanForSourceFilesInDir(root);
        for (std::string newFile : newFiles)
            files.push_back(newFile);
    }

    return files;
}


void findPackages(json *libraries, std::string file){
    std::ifstream f(file);
    if (!f.is_open()){
        std::cerr << "Could not open file `" << file << "` for library finding\n";
    }

    std::string data;
    std::string line;

    while (std::getline(f, line)){
        data += line + "\n";
    }

    QString qData = QString::fromStdString(data);

    if (qData.contains("qt5/")){
        libraries->push_back("qt5widgets");
        libraries->push_back("qt5core");
        libraries->push_back("qt5gui");
    }
    if (qData.contains("gtk-3.0/")) libraries->push_back("gtk3");
    if (qData.contains("gtk-4.0/")) libraries->push_back("gtk4");
    if (qData.contains("raylib.h")) libraries->push_back("raylib");
    if (qData.contains("SDL2/")) libraries->push_back("SDL2");
}

/****************************/
/****************************/

int build(QString fileName)
{
    std::ifstream file(fileName.toStdString());

    if (!file.is_open())
    {
        std::cerr << "Could not load file!\n";
        return -1;
    }

    json j;
    file >> j;

    if (!j.contains("project"))
    {
        std::cerr << "Project name not specified.\n";
        return -1;
    }

    if (!j.contains("root"))
    {
        std::cerr << "Build file must contain a `root` entry that has the path of the project dir.";
        return -1;
    }

    std::string projectName = j["project"];
    std::string projectRoot = j["root"];
    if (!QString::fromStdString(projectRoot).endsWith("/"))
        projectRoot += '/';

    QString qProjectName(QString::fromStdString(projectName));
    QString executableName(qProjectName);
    QString outputDirectory("./");
    json cacheFile;
    json previousCacheFile;

    if (j.contains("exe"))
    {
        executableName = QString::fromStdString(j["exe"]);

        if (executableName.contains("/"))
        {
            logWarning("Executable contains output directory. Use outputDir entry instead");
        }
    }

    if (j.contains("outputDir"))
    {
        std::string outputDir = j["outputDir"];
        QString qOutputDir(QString::fromStdString(outputDir));

        if (!qOutputDir.endsWith("/"))
        {
            qOutputDir.append("/");
        }

        outputDirectory = qOutputDir;
        logInfo("Output directory: " + qOutputDir.toStdString());
    }

    if (!fs::exists("./createc_cache.json"))
    {
        createCacheFile(qProjectName, QString("%1%2").arg(outputDirectory, executableName));
    }
    else
    {
        previousCacheFile = loadCacheFile();
        std::system("rm -rf ./createc_cache.json");
        createCacheFile(qProjectName, QString("%1%2").arg(outputDirectory, executableName));
        cacheFile = loadCacheFile();
    }

    auto sourcefiles = scanForSourceFiles(projectRoot);
    json libraries = json::array();

    if (j.contains("sources")){
        for (auto file : j["sources"]){
            sourcefiles.push_back(file);
        }
    }

    for (auto file : sourcefiles){
        findPackages(&libraries, file);
    }

    // Optional
    int optimizationLevel = 0;
    int cppStandard = 11;

    QString options;

    json scriptsToRunAfter;
    json scriptsToRunBefore;

    if (j.contains("cleanBuild"))
    {
        bool doACleanBuild = j["cleanBuild"];

        if (doACleanBuild)
        {
            if (!previousCacheFile.contains("executable"))
            {
                logError("Could not do a cleanBuild, No executable found in cache file (cache file might not exist. Try removing 'cleanBuild' temporarily.)");
                return -1;
            }

            std::system(QString("rm -rf ./%1").arg(QString::fromStdString(previousCacheFile["executable"])).toStdString().c_str());
        }
    }

    if (j.contains("compilerOptions"))
    {
        options = QString::fromStdString(j["compilerOptions"]);
        logInfo("Compiler options: " + options.toStdString());
    }

    if (j.contains("optlvl"))
    {
        optimizationLevel = j["optlvl"];
        if (optimizationLevel < 1 || optimizationLevel > 3)
        {
            std::cerr << "Invalid optimization level!\n";
            return -1;
        }

        logInfo(QString("Set optimization level to %1.").arg(optimizationLevel).toStdString());
    }

    if (j.contains("libraries"))
    {
        libraries = j["libraries"];
        for (const auto &lib : libraries)
        {
            QString text = QString("Added library `%1` to executable.").arg(QString::fromStdString(lib));
            logInfo(text.toStdString());
        }
    }

    if (j.contains("cppstd"))
    {
        int standard = j["cppstd"];

        if (standard != 11 && standard != 14 && standard != 17 && standard != 20 && standard != 23)
        {
            logError("C standard is invalid!");
            return -1;
        }

        cppStandard = standard;
        logInfo("Set clang standard to " + std::to_string(cppStandard));
    }

    /* END */
    /* SCRIPTS */
    if (j.contains("scripts"))
    {
        auto scripts = j["scripts"];

        if (scripts.contains("after"))
        {
            scriptsToRunAfter = scripts["after"];
        }

        if (scripts.contains("before"))
        {
            scriptsToRunBefore = scripts["before"];
            runScripts(scriptsToRunBefore);
        }
    }

    // Make and run the command
    QString command("g++ -o ");
    command.append(QString("%1%2").arg(outputDirectory, executableName));
    command.append(QString::fromStdString(" -std=c++" + std::to_string(cppStandard) + " "));

    for (const auto source : sourcefiles)
    {
        findPackages(&libraries, source);
        command.append(QString::fromStdString(source));
        command.append(" ");
    }

    if (j.contains("verbose"))
    {
        if (j["verbose"] == true)
        {
            command.append("-v ");
        }
    }

    if (j.contains("debugMode"))
    {
        if (j["debugMode"])
        {
            command.append("-g ");
        }
    }

    if (optimizationLevel > 0)
    {
        command.append("-O");
        command.append(QString::fromStdString(std::to_string(optimizationLevel)));
        command.append(" ");
    }

    if (!libraries.empty())
    {
        for (const auto lib : libraries)
        {
            doLibraryThing(&command, lib);
        }
    }

    
    std::cout << "\033[33mBuilding started.. [50%]\033[0m\n";
    
    // execute
    int result = std::system(command.toStdString().c_str());
    if (result != 0)
    {
        std::cout << errorColor + "Building completed with errors.\033[0m\n";
        return -1;
    }

    if (!scriptsToRunAfter.empty())
    {
        runScripts(scriptsToRunAfter);
    }

    std::cout << "\033[32mBuilding completed. [100%]\033[0m\n";
    return 0;
}

/****************************/
/****************************/