#ifndef CREATEGEN_H
#define CREATEGEN_H

#include <fstream>
#include <json.hpp>
#include <iostream>
using json = nlohmann::json;

inline void generateMinimalCreateCPPConfiguration(std::string project, std::string root, int cppstd){
    json j;
    j["project"] = project;
    j["root"] = root;
    j["cppstd"] = cppstd;

    std::ofstream file(root + "create.json");
    if (!file.is_open()){
        std::cerr << "Could not generate JSON configuration!\nTry writing the JSON file yourself, you lazy ahh.";
        std::exit(1);
    }

    file << j.dump(4);
    std::cout << "Generated config file: " + (root + "create.json") + "\n.";
}


#endif