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
        std::vector<std::pair<std::string, std::vector<std::string>>> css_folders_;

    };
}