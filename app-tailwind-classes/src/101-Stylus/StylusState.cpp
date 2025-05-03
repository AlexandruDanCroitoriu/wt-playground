#include "101-Stylus/StylusState.h"
#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include <Wt/WIOService.h>
#include <Wt/WRandom.h>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace Stylus {

    StylusState::StylusState()
    {
        doc_.LoadFile(file_path_.c_str());
        if (doc_.ErrorID() != tinyxml2::XML_SUCCESS)
        {
            std::cerr << "Error loading XML file: " << doc_.ErrorID() << std::endl;
            // crete file 
            std::ofstream file(file_path_);
            if (!file.is_open())
            {
                std::cerr << "Error creating file: " << file_path_ << std::endl;
                return;
            }
            doc_.LoadFile(file_path_.c_str());
        }
        stylus_open_node_ = doc_.FirstChildElement("stylus");
        if (stylus_open_node_ == nullptr)
        {
            std::cerr << "Error finding <stylus> node in XML file." << std::endl;
            // create the node
            stylus_open_node_ = doc_.NewElement("stylus");
            stylus_open_node_->SetAttribute("selected-menu", "templates");
            stylus_open_node_->SetAttribute("open", "true");
            stylus_open_node_->SetAttribute("navigation-bar-hidden", "false");
            stylus_open_node_->SetAttribute("dark-mode", "true");
            doc_.InsertFirstChild(stylus_open_node_);
        }
        xml_node_ = stylus_open_node_->FirstChildElement("xml-manager");
        if (xml_node_ == nullptr)
        {
            std::cerr << "Error finding <xml-manager> node in XML file." << std::endl;
            // create the node
            xml_node_ = doc_.NewElement("xml-manager");
            xml_node_->SetAttribute("editor-width", 300);
            xml_node_->SetAttribute("sidebar-width", 300);
            stylus_open_node_->InsertEndChild(xml_node_);
        }
        css_node_ = stylus_open_node_->FirstChildElement("css-manager");
        if (css_node_ == nullptr)
        {
            std::cerr << "Error finding <css-manager> node in XML file." << std::endl;
            // create the node
            css_node_ = doc_.NewElement("css-manager");
            css_node_->SetAttribute("sidebar-width", 300);
            stylus_open_node_->InsertEndChild(css_node_);
        }
        js_node_ = stylus_open_node_->FirstChildElement("js-manager");
        if (js_node_ == nullptr)
        {
            std::cerr << "Error finding <js-manager> node in XML file." << std::endl;
            // create the node
            js_node_ = doc_.NewElement("js-manager");
            js_node_->SetAttribute("sidebar-width", 300);
            stylus_open_node_->InsertEndChild(js_node_);
        }
        // save the file
        tailwind_config_node_ = stylus_open_node_->FirstChildElement("tailwind-config");
        if (tailwind_config_node_ == nullptr)
        {
            std::cerr << "Error finding <tailwind-config> node in XML file." << std::endl;
            // create the node
            tailwind_config_node_ = doc_.NewElement("tailwind-config");
            tailwind_config_node_->SetAttribute("selected-file-name", "");
            stylus_open_node_->InsertEndChild(tailwind_config_node_);
        }

        doc_.SaveFile(file_path_.c_str());
        if (doc_.ErrorID() != tinyxml2::XML_SUCCESS)
        {
            std::cerr << "Error saving XML file: " << doc_.ErrorID() << std::endl;
        }
        std::cout << "\n\n stylus state loaded \n\n";
    }


}