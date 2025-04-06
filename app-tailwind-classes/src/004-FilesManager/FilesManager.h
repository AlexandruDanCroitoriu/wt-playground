#pragma once
#include <Wt/WContainerWidget.h>
#include "004-FilesManager/MonacoEdditor.h"

class FilesManager : public Wt::WContainerWidget
{
    public:
        FilesManager(std::string default_folder_path, std::string language);
        MonacoEdditor* editor_;
        std::vector<std::pair<std::string, std::vector<std::string>>> folders_;
        std::string default_folder_path_;
private:
        void setTreeFolderWidgets();
        Wt::WMessageBox* createMessageBox(std::string title, std::string temp);

        Wt::WContainerWidget* selected_file_wrapper_;
        Wt::WContainerWidget* folders_tree_wrapper_;
        Wt::WText* tree_header_title_;
        std::vector<std::pair<std::string, std::vector<std::string>>> getCssFolders();
};