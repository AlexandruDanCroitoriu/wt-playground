#pragma once
#include <Wt/WString.h>
#include "100-Utils/tinyxml2/tinyxml2.h"

namespace Stylus
{
    struct StylusState {
        StylusState();
        tinyxml2::XMLDocument doc_;
        std::string file_path_ = "../stylus-state.xml";
        tinyxml2::XMLElement* stylus_open_node_ = nullptr;
        tinyxml2::XMLElement* xml_node_ = nullptr;
        tinyxml2::XMLElement* css_node_ = nullptr;
        tinyxml2::XMLElement* js_node_ = nullptr;
    };

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

        StylusState state_;
    };
}