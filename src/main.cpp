#include <iostream>
#include <cxxopts.hpp>
#include <string>

// #include <cli11>
#include "RTMesh.h"
#include <filesystem>


int main(int argc, const char *argv[]) {

    // parse cli
    // Define options
    cxxopts::Options options("obj2bin", "A tool to convert OBJ files to binary format");
    options.add_options("")
        ("o,output", "output", cxxopts::value<std::string>()->default_value("")) // string
        ("t,test", "Enable test mode", cxxopts::value<bool>()) // boolean, implicitly false
        ("h,help", "Print usage");

    // Define positional parameters
    options.add_options()
        ("file", "Input file name (required)", cxxopts::value<std::vector<std::string>>());

    // Specify positional arguments
    options.parse_positional({"file"});

    // Customize help text
    options.custom_help("[OPTIONS...]");
    options.positional_help("<input_files>");

    // parse
    cxxopts::ParseResult result;
    try {
        // Parse command-line arguments
        result = options.parse(argc, argv);
    } catch (const cxxopts::exceptions::parsing& e) {
        // Handle parsing exceptions
        std::cerr << "Error parsing options: " << e.what() << std::endl;
        std::cout << options.help() << std::endl;
        return 1;
    }

    // check para
    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }
    if (!result.count("file")) {
        std::cerr << "Error: No input file provided." << std::endl;
        // std::cout << options.help({"Options", "Positional options" }) << std::endl;
        std::cout << options.help() << std::endl;
        return 1;
    }

    // Retrieve and display the input file(s)
    const auto& files = result["file"].as<std::vector<std::string>>();
    for (const auto& file : files) {
        std::cout << "Input file: " << file << std::endl;
    }

    // Retrieve optional output parameter
    if (result.count("output")) {
        std::cout << "Output file: " << result["output"].as<std::string>() << std::endl;
    }

    // std::cout << "begin load file: " << files[0] << std::endl;
    auto mesh = RTMesh::LoadObj(files[0]);
    // std::cout << "load file done \n";
    auto outfile = result["output"].as<std::string>();
    if (outfile == "") {
        auto filePath =  std::filesystem::path(files[0]);
        std::filesystem::path dir = filePath.parent_path();
        std::filesystem::path fileName = filePath.stem();           // fileName() with .obj stemp no .obj
        std::filesystem::path fileExtension = filePath.extension(); // .obj
        std::filesystem::path fullpath = (dir / fileName);
        // std::cout << "fileName:" << fileName << "\n";
        // std::cout << "fileExtension:" << fileExtension << "\n";
        fullpath += ".bin";
        outfile = fullpath.string();
    }
    mesh.SaveToBin(outfile);


    if (result.count("test")) {
        std::cout << "test\n";

        std::string testfile;
        {
            auto filePath =  std::filesystem::path(files[0]);
            std::filesystem::path dir = filePath.parent_path();
            std::filesystem::path fileName = filePath.stem();           // fileName() with .obj stemp no .obj
            std::filesystem::path fileExtension = filePath.extension(); // .obj
            std::filesystem::path fullpath = (dir / fileName);
            // std::cout << "fileName:" << fileName << "\n";
            // std::cout << "fileExtension:" << fileExtension << "\n";
            fullpath += "_test.obj";
            testfile = fullpath.string();
        }

        RTMesh mesh =  RTMesh::LoadBin(outfile);
        mesh.SaveToObj(testfile);
    }

    return 0;
}