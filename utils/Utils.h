#pragma once

#include <string>
#include <fstream>
#include <stdexcept>

inline void createFileWithContent(const std::string& filePath, const std::string& content) {
    std::ofstream outFile;
    if (std::ifstream(filePath)) {
        outFile.open(filePath, std::ios::app); // Open in append mode if file exists
    } else {
        outFile.open(filePath); // Create a new file if it doesn't exist
    }
    if (!outFile) {
        throw std::runtime_error("Failed to open or create file at: " + filePath);
    }
    outFile << content;
    outFile.close();
}