#include "utils.hpp"

#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;

std::vector<std::string> getAssetsFilenames(const std::string& directoryPath) {
    std::vector<std::string> filenames;
    for (const auto &entry : fs::directory_iterator(directoryPath)){
        if (entry.is_regular_file()) {
            filenames.push_back(entry.path().string());
        }
    }
    return filenames;
}