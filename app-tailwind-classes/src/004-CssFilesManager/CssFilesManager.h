#pragma once
#include <Wt/WContainerWidget.h>
#include "004-CssFilesManager/MonacoCssEdditor.h"

class CssFilesManager : public Wt::WContainerWidget
{
    public:
        CssFilesManager();
        MonacoCssEdditor* css_editor_;
        std::vector<std::pair<std::string, std::vector<std::string>>> css_folders_;
        std::string default_css_path_ = "../../tailwind4/css/";
        std::string selected_css_file_; 
private:
    Wt::WContainerWidget* selected_css_file_wrapper_;

    std::vector<std::pair<std::string, std::vector<std::string>>> getCssFolders();
};