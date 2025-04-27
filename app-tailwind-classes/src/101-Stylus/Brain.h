#pragma once
#include <Wt/WString.h>

namespace Stylus
{
    class Brain
    {
    public:
        Brain();
        void generateCssFile();
        Wt::WString tailwind_config_ = "";
        std::shared_ptr<std::vector<std::pair<std::string, std::vector<std::string>>>> css_folders_;
        // std::vector<std::pair<std::string, std::vector<std::string>>> css_folders_;
        Wt::WString current_css_file_;
        Wt::WString prev_css_file_;
    };
}