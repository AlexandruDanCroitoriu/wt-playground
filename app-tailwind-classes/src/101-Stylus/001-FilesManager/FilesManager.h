#pragma once
#include <Wt/WContainerWidget.h>
#include "101-Stylus/002-MonacoEditor/MonacoEdditor.h"

namespace Stylus {

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
        std::string file_extension_;
        std::string selected_file_path_;
        Wt::WContainerWidget* selected_file_wrapper_;
        Wt::WContainerWidget* folders_tree_wrapper_;
        Wt::WText* tree_header_title_;
        std::vector<std::pair<std::string, std::vector<std::string>>> getFolders();
};

}