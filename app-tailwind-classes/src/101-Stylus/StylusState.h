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
        tinyxml2::XMLElement* tailwind_config_node_ = nullptr;
    
    };

}