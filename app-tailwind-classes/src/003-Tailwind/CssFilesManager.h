#pragma once
#include <Wt/WContainerWidget.h>
#include "003-Tailwind/MonacoCssEdditor.h"

class CssFilesManager : public Wt::WContainerWidget
{
    public:
        CssFilesManager();
        MonacoCssEdditor* css_editor_;
private:
    std::string default_css_path_ = "../../tailwind4/css/";
    std::string default_css_file_ = "default.css";
    std::string selected_css_file_; 

    std::vector<std::pair<std::string, std::vector<std::string>>> getCssFolders();
};