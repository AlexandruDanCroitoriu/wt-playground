#include "101-Stylus/Brain.h"
#include <Wt/WApplication.h>
#include <Wt/WServer.h>
#include <Wt/WIOService.h>
#include <Wt/WRandom.h>
#include <fstream>

namespace Stylus {

    Brain::Brain()
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