#include "003-Tailwind/CssFilesManager.h"
#include <filesystem>

CssFilesManager::CssFilesManager()
{
    css_editor_ = addWidget(std::make_unique<MonacoCssEdditor>());


    auto css_folders = getCssFolders();
    for(const auto& folder : css_folders)
    {
        for(const auto& file : folder.second)
        {
            std::cout << "Folder: " << folder.first << ", File: " << file << "\n";
        }
    }   
}

std::vector<std::pair<std::string, std::vector<std::string>>> CssFilesManager::getCssFolders()
{
    std::vector<std::pair<std::string, std::vector<std::string>>> css_folders;
    std::vector<std::string> folders;
    for (const auto& entry : std::filesystem::directory_iterator(default_css_path_)) {
        if (entry.is_directory()) {
            folders.push_back(entry.path().filename().string());
        }
    }
    for(const auto& folder : folders)
    {
        std::vector<std::string> files;
        for (const auto& entry : std::filesystem::directory_iterator(default_css_path_ + folder)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().filename().string());
            }
        }
        css_folders.push_back(std::make_pair(folder, files));
    }
    for(const auto& folder : css_folders)
    {
        for(const auto& file : folder.second)
        {
            std::cout << "Folder: " << folder.first << ", File: " << file << "\n";
        }
    }
    return css_folders;
}
