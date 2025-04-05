#pragma once
#include <Wt/WContainerWidget.h>
#include "004-FilesManagers/MonacoEdditor.h"

class CssFilesManager : public Wt::WContainerWidget
{
    public:
        CssFilesManager();
        MonacoEdditor* editor_;
        std::vector<std::pair<std::string, std::vector<std::string>>> folders_;
        std::string default_css_path_ = "../../tailwind4/css/";
        std::string selected_css_file_; 
private:
        void setTreeFolderWidgets();

        Wt::WContainerWidget* selected_css_file_wrapper_;
        Wt::WContainerWidget* folders_tree_wrapper_;
        Wt::WText* tree_header_title_;
        std::vector<std::pair<std::string, std::vector<std::string>>> getCssFolders();
};