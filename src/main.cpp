#include <iostream>
#include <cxxopts.hpp>
#include <string>
#include <fstream>
#include <filesystem>
#include <fmt/core.h>
// #include <cli11>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

struct Mesh {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<uint32_t> indices;
};


static Mesh loadObj(const std::string& file) {
    const std::string inputfile = file;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

    Mesh mesh;

    if (!warn.empty()) {
        std::cout << "Warning: " << warn << std::endl;
    }
    if (!err.empty()) {
        std::cerr << "Error: " << err << std::endl;
    }
    if (!success) {
        return mesh; // Load failed
    }

    mesh.vertices = std::move(attrib.vertices);
    mesh.normals = std::move(attrib.normals);

    // Calculate total number of indices across all shapes
    size_t totalIndices = 0;
    for (const auto& shape : shapes) {
        totalIndices += shape.mesh.indices.size();
    }
    mesh.indices.resize(totalIndices);
    
    // Assign indices from all shapes
    size_t currentIndex = 0;
    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            mesh.indices[currentIndex++] = index.vertex_index;
        }
    }

    return mesh;
}


static void saveToObj(const std::string& filename, Mesh& mesh) {

    const std::vector<float>& vertices = mesh.vertices;
    const std::vector<float>& normals = mesh.normals;
    const std::vector<unsigned int>& indices = mesh.indices;

    std::ofstream outFile(filename);
    if (!outFile) {
        throw std::runtime_error("Failed to open file for writing");
    }

    // Writing vertices
    for (size_t i = 0; i < vertices.size(); i += 3) {
        outFile << "v " 
                << fmt::format("{:f}", vertices[i + 0]) << " " 
                << fmt::format("{:f}", vertices[i + 1]) << " " 
                << fmt::format("{:f}", vertices[i + 2]) << "\n";
    }

    // Writing normals
    for (size_t i = 0; i < normals.size(); i += 3) {
        outFile << "vn " 
                <<  fmt::format("{:f}", normals[i + 0]) << " " 
                <<  fmt::format("{:f}", normals[i + 1]) << " " 
                <<  fmt::format("{:f}", normals[i + 2]) << "\n";
    }

    // Writing indices (triangles)
    const size_t triangles = indices.size() / 3;
    size_t nnormal = normals.size();
    for (size_t i = 0; i < triangles; ++i) {
        std::string i0 = fmt::format("{:d}", indices[i * 3 + 0] + 1);
        std::string i1 = fmt::format("{:d}", indices[i * 3 + 1] + 1);
        std::string i2 = fmt::format("{:d}", indices[i * 3 + 2] + 1);
        if (nnormal != 0) {
            outFile << "f "
                    << i0 << "//" << i0 << " "
                    << i1 << "//" << i1 << " "
                    << i2 << "//" << i2 << "\n";
        } else {
            outFile << "f "
                    << i0  << " "
                    << i1  << " "
                    << i2  << "\n";
        }
    }

    outFile.close();

    std::cout << "Successfully saved to " << filename << std::endl;
}


static void saveToLine(const std::string& filename, std::vector<float>& lines) {

    const std::vector<float>& vertices = lines;

    std::ofstream outFile(filename);

    if (!outFile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    // Write vertices
    for (size_t i = 0; i < vertices.size(); i += 3) {
        outFile << "v " 
                << vertices[i] << " " 
                << vertices[i + 1] << " " 
                << vertices[i + 2] << "\n";
    }

    
    // Write faces (indices)
    for (size_t i = 0; i < vertices.size() / 3 / 2; i ++) {
        // OBJ uses 1-based indexing
        outFile << "l " 
                << i * 2 + 0 + 1 << " "
                << i * 2 + 1 + 1 << "\n";
    }

    outFile.close();

    std::cout << "Successfully saved to " << filename << std::endl;
}

static void saveToPly(const std::string& filename, std::vector<float>& points) {
     size_t numPoints = points.size() / 3;

      // Open the output file
    std::ofstream outFile(filename, std::ios::out);
    if (!outFile) {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return;
    }

    // Write the PLY header
    outFile << "ply\n";
    outFile << "format ascii 1.0\n";
    outFile << "element vertex " << numPoints << "\n";
    outFile << "property float x\n";
    outFile << "property float y\n";
    outFile << "property float z\n";
    outFile << "end_header\n";

    // Write the points
    for (size_t i = 0; i < numPoints; ++i) {
        outFile << points[i * 3 + 0] << " "  // x
                << points[i * 3 + 1] << " "  // y
                << points[i * 3 + 2] << "\n"; // z
    }

    outFile.close();
    std::cout << "PLY file written to " << filename << std::endl;
}

static void saveToBin(const std::string& filename, const Mesh& mesh) {
    // Open file in binary mode
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }

    // Write vertex count and index count
    uint32_t vertexCount = static_cast<uint32_t>(mesh.vertices.size() / 3);
    uint32_t indexCount = static_cast<uint32_t>(mesh.indices.size());
    outFile.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));
    outFile.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));

    // Write vertex data
    outFile.write(reinterpret_cast<const char*>(mesh.vertices.data()), mesh.vertices.size() * sizeof(float));

    // Write normal data
    outFile.write(reinterpret_cast<const char*>(mesh.normals.data()), mesh.normals.size() * sizeof(float));

    // Write index data
    outFile.write(reinterpret_cast<const char*>(mesh.indices.data()), mesh.indices.size() * sizeof(uint32_t));

    outFile.close();

    std::cout << "Successfully saved to " << filename << std::endl;
    std::cout << "vertices count: " << vertexCount << " normals count: " << mesh.normals.size() / 3 << " indices count: " << indexCount << std::endl;
}

static Mesh loadBin(const std::string& filename) {
    Mesh mesh;

    // Open file in binary mode
    std::ifstream inFile(filename, std::ios::binary | std::ios::ate);
    if (!inFile.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filename);
    }

    // Get file size
    std::streamsize fileSize = inFile.tellg();
    inFile.seekg(0, std::ios::beg); // Move back to start

    // Read vertex count and index count
    uint32_t vertexCount = 0;
    uint32_t indexCount = 0;
    inFile.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
    inFile.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));

    // Calculate size needed for vertices and indices
    std::streamsize sizeForVertices = vertexCount * 3 * sizeof(float);
    std::streamsize sizeForIndices = indexCount * sizeof(uint32_t);
    std::streamsize sizeForNormals = vertexCount * 3 * sizeof(float);
    std::streamsize minSize = sizeof(uint32_t) + sizeof(uint32_t) + sizeForVertices + sizeForIndices;

    // Resize vectors
    mesh.vertices.resize(vertexCount * 3); // 3 floats per vertex
    mesh.normals.resize(0);                // 3 floats per normal
    mesh.indices.resize(indexCount);       // 1 uint32_t per index

    // Read vertex data
    inFile.read(reinterpret_cast<char*>(mesh.vertices.data()), mesh.vertices.size() * sizeof(float));

    // Read normal data
    if (fileSize >= minSize + sizeForNormals) {
        mesh.normals.resize(vertexCount * 3);   // 3 floats per normal
        inFile.read(reinterpret_cast<char*>(mesh.normals.data()), mesh.normals.size() * sizeof(float));
    }

    // Read index data
    inFile.read(reinterpret_cast<char*>(mesh.indices.data()), mesh.indices.size() * sizeof(uint32_t));

    inFile.close();

    return mesh;
}

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

    auto mesh = loadObj(files[0]);
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
    saveToBin(outfile, mesh);


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

        Mesh mesh =  loadBin(outfile);
        saveToObj(testfile, mesh);
    }

    std::cout << "hello world!\n";
    return 0;
}

