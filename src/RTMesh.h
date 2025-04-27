/// @file      RTMesh.h
/// @brief     
/// @copyright 
/// @par       
#pragma once
#include <vector>
#include <memory>
#include <string>

struct RTMesh {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<uint32_t> indices;
    std::vector<std::string> group_names; // Unique group names (e.g., "group_1")
    std::vector<uint32_t> group_ids;      // Group ID per face, id is index of group_names
    std::vector<uint32_t> group_ranges;   // Start index of each group in indices

    static RTMesh LoadObj(const std::string& file);
    void SaveToObj(const std::string& filename);
    static void SaveToLine(const std::string& filename, std::vector<float>& lines);
    static void SaveToPly(const std::string& filename, std::vector<float>& points);
    void SaveToBin(const std::string& filename);
    static RTMesh LoadBin2_0(const std::string& filename);
    static RTMesh LoadBin(const std::string& filename);
};
