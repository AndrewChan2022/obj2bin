/// @file      RTMesh.cpp
/// @brief     
/// @copyright Copyright (c) 2025 Hefei Jiushao Intelligent Technology Co., Ltd. All rights reserved.
/// @par       This file is part of next_gsp.
#include "RTMesh.h"
#include <fstream>
#include <filesystem>
#include <fmt/core.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <iostream>
#include <filesystem>
#include <fstream>

#include <cmath>


// Define M_PI if not already defined
#ifndef M_PI
#define M_PI 3.141592653589793f
#endif


RTMesh RTMesh::LoadObj(const std::string& file) {
    const std::string inputfile = file;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool success = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

    RTMesh mesh;

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

    // Compute sizes for indices, group_ids, group_names, and group_ranges
    size_t totalIndices = 0;
    size_t totalFaces = 0;
    size_t validShapes = 0;
    for (const auto& shape : shapes) {
        size_t shapeIndices = 0;
        size_t shapeFaces = shape.mesh.num_face_vertices.size();
        for (size_t f = 0; f < shapeFaces; ++f) {
            if (shape.mesh.num_face_vertices[f] == 3) {
                shapeIndices += 3; // Triangular face
                ++totalFaces;
            }
        }
        if (shapeIndices > 0) { // Only count shapes with valid triangular faces
            totalIndices += shapeIndices;
            ++validShapes;
        }
    }
    mesh.indices.resize(totalIndices);
    mesh.group_ids.resize(totalFaces);
    mesh.group_names.resize(validShapes);
    mesh.group_ranges.resize(validShapes + 1); // +1 for end marker
    
    // Process each shape (group)
    uint32_t currentIndex = 0;
    uint32_t currentFace = 0;
    uint32_t currentGroup = 0;
    for (const auto& shape : shapes) {
        const std::string& groupName = shape.name.empty() ? "default_group" : shape.name;
        size_t shapeIndices = 0;
        size_t shapeFaces = 0;

        // Count valid indices and faces for this shape
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
            if (shape.mesh.num_face_vertices[f] == 3) {
                shapeIndices += 3;
                ++shapeFaces;
            }
        }

        if (shapeIndices == 0) {
            continue; // Skip shapes with no triangular faces
        }

        // Assign group name and range
        mesh.group_names[currentGroup] = groupName;
        mesh.group_ranges[currentGroup] = currentIndex;

        // Process faces in the shape
        size_t indexOffset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
            int fv = shape.mesh.num_face_vertices[f];
            if (fv != 3) {
                std::cerr << "Warning: Non-triangular face detected, skipping." << std::endl;
                indexOffset += fv;
                continue;
            }

            // Assign indices for the face
            for (int v = 0; v < fv; ++v) {
                const auto& idx = shape.mesh.indices[indexOffset + v];
                mesh.indices[currentIndex + v] = idx.vertex_index;
            }

            // Assign group ID to this face
            mesh.group_ids[currentFace] = currentGroup;

            indexOffset += fv;
            currentIndex += fv;
            ++currentFace;
        }

        ++currentGroup;
    }
    // Assign end marker for group ranges
    mesh.group_ranges[currentGroup] = currentIndex;

    return mesh;
}


void RTMesh::SaveToObj(const std::string& filename) {

    RTMesh& mesh = *this;

    const std::vector<float>& vertices = mesh.vertices;
    const std::vector<float>& normals = mesh.normals;
    const std::vector<unsigned int>& indices = mesh.indices;
    const std::vector<std::string>& group_names = mesh.group_names;
    const std::vector<uint32_t>& group_ids = mesh.group_ids;
    const std::vector<uint32_t>& group_ranges = mesh.group_ranges;      // helper info, please not depend on it

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

    // Write faces, with group names only if multiple groups and group_ids changes
    const size_t numGroups = group_names.size();
    const bool hasNormals = !normals.empty();
    const bool writeGroups = numGroups > 1;
    uint32_t lastGroupId = std::numeric_limits<uint32_t>::max(); // Invalid initial value

    for (size_t faceIdx = 0; faceIdx < group_ids.size(); ++faceIdx) {
        const uint32_t currentGroupId = group_ids[faceIdx];

        // Write group name if group_id changes and multiple groups exist
        if (writeGroups && currentGroupId != lastGroupId) {
            outFile << "g " << group_names[currentGroupId] << "\n";
            lastGroupId = currentGroupId;
        }

        // Write face (OBJ indices are 1-based, 3 indices per face)
        const uint32_t idx = faceIdx * 3;
        std::string i0 = fmt::format("{:d}", indices[idx + 0] + 1);
        std::string i1 = fmt::format("{:d}", indices[idx + 1] + 1);
        std::string i2 = fmt::format("{:d}", indices[idx + 2] + 1);

        if (hasNormals) {
            outFile << fmt::format("f {}//{} {}//{} {}//{}\n", i0, i0, i1, i1, i2, i2);
        } else {
            outFile << fmt::format("f {} {} {}\n", i0, i1, i2);
        }
    }

    outFile.close();

    std::cout << "Successfully saved to " << filename << std::endl;
}


void RTMesh::SaveToLine(const std::string& filename, std::vector<float>& lines) {

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

void RTMesh::SaveToPly(const std::string& filename, std::vector<float>& points) {
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

void RTMesh::SaveToBin(const std::string& filename) {

    const RTMesh& mesh = *this;

    // Open file in binary mode
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + filename);
    }

    // Write magic number
    const char magicNumber[] = "objbin"; // 6 bytes, not null-terminated
    outFile.write(magicNumber, 6);

    // Write version
    uint32_t majorVersion = 2;
    uint32_t minorVersion = 1;
    uint32_t patchVersion = 0;
    outFile.write(reinterpret_cast<const char*>(&majorVersion), sizeof(majorVersion));
    outFile.write(reinterpret_cast<const char*>(&minorVersion), sizeof(minorVersion));
    outFile.write(reinterpret_cast<const char*>(&patchVersion), sizeof(patchVersion));

    // Compute group names section size
    uint32_t groupNamesSize = 0;
    for (size_t i = 0; i < mesh.group_names.size(); ++i) {
        groupNamesSize += 4; // nameLength field (uint32_t)
        groupNamesSize += static_cast<uint32_t>(mesh.group_names[i].size()); // name data
    }

    // Write header counts
    uint32_t vertexCount = static_cast<uint32_t>(mesh.vertices.size() / 3);
    uint32_t normalCount = static_cast<uint32_t>(mesh.normals.size() / 3);
    uint32_t indexCount = static_cast<uint32_t>(mesh.indices.size());
    uint32_t groupNameCount = static_cast<uint32_t>(mesh.group_names.size());
    uint32_t faceCount = static_cast<uint32_t>(mesh.group_ids.size());

    outFile.write(reinterpret_cast<const char*>(&vertexCount), sizeof(vertexCount));
    outFile.write(reinterpret_cast<const char*>(&normalCount), sizeof(normalCount));
    outFile.write(reinterpret_cast<const char*>(&indexCount), sizeof(indexCount));
    outFile.write(reinterpret_cast<const char*>(&groupNameCount), sizeof(groupNameCount));
    outFile.write(reinterpret_cast<const char*>(&groupNamesSize), sizeof(groupNamesSize));
    outFile.write(reinterpret_cast<const char*>(&faceCount), sizeof(faceCount));

    // Write vertex data
    outFile.write(reinterpret_cast<const char*>(mesh.vertices.data()), mesh.vertices.size() * sizeof(float));

    // Write normal data
    outFile.write(reinterpret_cast<const char*>(mesh.normals.data()), mesh.normals.size() * sizeof(float));

    // Write index data
    outFile.write(reinterpret_cast<const char*>(mesh.indices.data()), mesh.indices.size() * sizeof(uint32_t));

    // Write group names section
    for (size_t i = 0; i < groupNameCount; ++i) {
        const std::string& name = mesh.group_names[i];
        uint32_t nameLength = static_cast<uint32_t>(name.size());
        outFile.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        outFile.write(name.data(), nameLength * sizeof(char));
    }

    // Write group IDs
    outFile.write(reinterpret_cast<const char*>(mesh.group_ids.data()), mesh.group_ids.size() * sizeof(uint32_t));

    outFile.close();

    std::cout << "Successfully saved to " << filename << std::endl;
    std::cout << "vertices count: " << vertexCount
              << " normals count: " << normalCount
              << " indices count: " << indexCount
              << " group names count: " << groupNameCount
              << " group names size: " << groupNamesSize
              << " faces count: " << faceCount << std::endl;

}

RTMesh RTMesh::LoadBin(const std::string& filename) {
    RTMesh mesh;

    // Open file in binary mode
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile.is_open()) {
        // throw std::runtime_error("Failed to open file for reading: " + filename);
        std::cout << "Failed to open file for reading: " + filename << std::endl;
        return mesh;
    }

    // Read and verify magic number
    char magicNumber[6];
    inFile.read(magicNumber, 6);
    if (!inFile || std::string(magicNumber, 6) != "objbin") {
        inFile.close();
        // throw std::runtime_error("Invalid file format: incorrect magic number in " + filename);
        std::cout << "Invalid file format: incorrect magic number in " + filename << std::endl;
        return RTMesh();
    }

    // Read version
    uint32_t majorVersion, minorVersion, patchVersion;
    inFile.read(reinterpret_cast<char*>(&majorVersion), sizeof(majorVersion));
    inFile.read(reinterpret_cast<char*>(&minorVersion), sizeof(minorVersion));
    inFile.read(reinterpret_cast<char*>(&patchVersion), sizeof(patchVersion));
    if (!inFile || majorVersion != 2 || minorVersion != 1 || patchVersion != 0) {
        inFile.close();
        // throw std::runtime_error("Unsupported file version: expected 2.1.0, got " +
        //                          std::to_string(majorVersion) + "." +
        //                          std::to_string(minorVersion) + "." +
        //                          std::to_string(patchVersion));
        std::cout << "Error: Unsupported file version: expected 2.1.0, got "
                  << majorVersion << "." << minorVersion << "." << patchVersion
                  << " in " << filename << std::endl;
        return RTMesh();

    }

    // Read header counts
    uint32_t vertexCount, normalCount, indexCount, groupNameCount, groupNamesSize, faceCount;
    inFile.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
    inFile.read(reinterpret_cast<char*>(&normalCount), sizeof(normalCount));
    inFile.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));
    inFile.read(reinterpret_cast<char*>(&groupNameCount), sizeof(groupNameCount));
    inFile.read(reinterpret_cast<char*>(&groupNamesSize), sizeof(groupNamesSize));
    inFile.read(reinterpret_cast<char*>(&faceCount), sizeof(faceCount));    

    // Resize vectors
    mesh.vertices.resize(vertexCount * 3);
    mesh.normals.resize(normalCount * 3);
    mesh.indices.resize(indexCount);
    mesh.group_names.resize(groupNameCount);
    mesh.group_ids.resize(faceCount);

    // Read data
    inFile.read(reinterpret_cast<char*>(mesh.vertices.data()), vertexCount * 3 * sizeof(float));
    inFile.read(reinterpret_cast<char*>(mesh.normals.data()), normalCount * 3 * sizeof(float));
    inFile.read(reinterpret_cast<char*>(mesh.indices.data()), indexCount * sizeof(uint32_t));

    // Read group names section
    uint32_t bytesRead = 0;
    for (uint32_t i = 0; i < groupNameCount; ++i) {
        // Read name length
        uint32_t nameLength;
        inFile.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength));
        bytesRead += 4;

        // Read name data
        std::vector<char> nameBuffer(nameLength);
        inFile.read(nameBuffer.data(), nameLength * sizeof(char));
        bytesRead += nameLength;

        // Assign to group_names
        mesh.group_names[i].assign(nameBuffer.data(), nameLength);
    }
    if (bytesRead != groupNamesSize) {
        inFile.close();
        // throw std::runtime_error("Group names section size mismatch: expected " +
        //                          std::to_string(groupNamesSize) + " bytes, read " +
        //                          std::to_string(bytesRead) + " bytes in " + filename);
        std::cout << "Error: Group names section size mismatch: expected " << groupNamesSize
                  << " bytes, read " << bytesRead << " bytes in " << filename << std::endl;
        return RTMesh();
    }

    // Read group IDs
    inFile.read(reinterpret_cast<char*>(mesh.group_ids.data()), faceCount * sizeof(uint32_t));

    inFile.close();

    // std::cout << "Successfully loaded from " << filename << std::endl;
    // std::cout << "vertices count: " << vertexCount
    //           << " normals count: " << normalCount
    //           << " indices count: " << indexCount
    //           << " group names count: " << groupNameCount
    //           << " group names size: " << groupNamesSize
    //           << " faces count: " << faceCount << std::endl;

    return mesh;
}

RTMesh RTMesh::LoadBin2_0(const std::string& filename) {
    RTMesh mesh;

    // Open file in binary mode
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + filename);
    }

    // Read counts
    uint32_t majorVersion = 0;
    uint32_t minorVersion = 0;
    uint32_t vertexCount = 0;
    uint32_t normalCount = 0;
    uint32_t indexCount = 0;
    inFile.read(reinterpret_cast<char*>(&majorVersion), sizeof(majorVersion));
    inFile.read(reinterpret_cast<char*>(&minorVersion), sizeof(minorVersion));
    inFile.read(reinterpret_cast<char*>(&vertexCount), sizeof(vertexCount));
    inFile.read(reinterpret_cast<char*>(&normalCount), sizeof(normalCount));
    inFile.read(reinterpret_cast<char*>(&indexCount), sizeof(indexCount));

    // Resize vectors based on counts
    mesh.vertices.resize(vertexCount * 3);
    mesh.normals.resize(normalCount * 3);
    mesh.indices.resize(indexCount);

    // Read vertex data
    inFile.read(reinterpret_cast<char*>(mesh.vertices.data()), mesh.vertices.size() * sizeof(float));

    // Read normal data
    inFile.read(reinterpret_cast<char*>(mesh.normals.data()), mesh.normals.size() * sizeof(float));

    // Read index data
    inFile.read(reinterpret_cast<char*>(mesh.indices.data()), mesh.indices.size() * sizeof(uint32_t));

    inFile.close();

    return mesh;
}
