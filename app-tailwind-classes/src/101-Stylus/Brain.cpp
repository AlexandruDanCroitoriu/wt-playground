#include "101-Stylus/Brain.h"
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
        doc_.SaveFile(file_path_.c_str());
        if (doc_.ErrorID() != tinyxml2::XML_SUCCESS)
        {
            std::cerr << "Error saving XML file: " << doc_.ErrorID() << std::endl;
        }
        std::cout << "\n\n stylus state loaded \n\n";
    }



    Brain::Brain()
        : state_()
    {
    }

    void Brain::generateCssFile()
    {
        // std::cout << "\n\n start writing file \n\n";

        std::ofstream file("../stylus-resources/tailwind4/input.css");
        if (!file.is_open())
        {
            std::cerr << "Error opening file for writing: " << "../stylus-resources/tailwind4/input.css" << std::endl;
            return;
        }   
        // std::cout << "\n\n file opened \n\n";
        file << "/* Import TailwindCSS base styles */\n";
        file << "@import \"tailwindcss\";\n\n";
        file << "/* Import custom CSS files for additional styles */\n";
        // std::cout << "\n\n writing file imports \n\n";
        for(const auto& folder : *css_folders_)
        {
            for (const auto& file_name : folder.second)
            {
                file << "@import \"./css/" << folder.first << "/" << file_name << "\";\n";
            }
        }

        file << "/* Source additional templates and styles */\n";
        file << "@source \"../xml-templates/\";\n";
        file << "@source \"../../src/\";\n";

        file << "/* Define custom variants */\n";
        file << "@custom-variant dark (&:where(.dark, .dark *));\n\n";

        file << "/* Define custom theme */\n";
        // std::cout << "\n\n writing tailwind config variables \n\n";
        file << tailwind_config_ << "\n";

        // std::cout << "\n\nFile written successfully: " << "../stylus-resources/tailwind4/input.css\n\n";
        file.close();

        // std::cout << "\n\nGenerating CSS file...\n\n";
        auto session_id = Wt::WApplication::instance()->sessionId();
        Wt::WServer::instance()->ioService().post([this, session_id](){
            std::system("cd ../stylus-resources/tailwind4 && npm run build");
            Wt::WServer::instance()->post(session_id, [this]() {
                current_css_file_ = Wt::WApplication::instance()->docRoot() + "/../static/tailwind.css?v=" + Wt::WRandom::generateId();
                Wt::WApplication::instance()->removeStyleSheet(prev_css_file_.toUTF8());
                Wt::WApplication::instance()->useStyleSheet(current_css_file_.toUTF8());
                prev_css_file_ = current_css_file_;
                Wt::WApplication::instance()->triggerUpdate();
            }); 
        });
    }


}