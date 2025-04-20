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

        std::ofstream file("../stylus-resources/tailwind4/input.css");
        if (!file.is_open())
        {
            std::cerr << "Error opening file for writing: " << "../stylus-resources/tailwind4/input.css" << std::endl;
            return;
        }   
        file << "/* Import TailwindCSS base styles */\n";
        file << "@import \"tailwindcss\";\n\n";
        file << "/* Import custom CSS files for additional styles */\n";
        for(const auto& folder : css_folders_)
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
        file << tailwind_config_ << "\n";

        file.close();
        std::cout << "File written successfully: " << "../stylus-resources/tailwind4/input.css" << std::endl;


        auto session_id = Wt::WApplication::instance()->sessionId();
        Wt::WServer::instance()->ioService().post([this, session_id]()
                                                  {
        std::system("cd ../stylus-resources/tailwind4 && npm run build");
        Wt::WServer::instance()->post(session_id, [this]() {
            Wt::WApplication::instance()->useStyleSheet(Wt::WApplication::instance()->docRoot() + "/../static/tailwind.css?v=" + Wt::WRandom::generateId());
            Wt::WApplication::instance()->triggerUpdate();
        }); });
    }


}