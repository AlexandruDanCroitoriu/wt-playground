#include "101-Stylus/002-CssFilesManager/CssFilesManager.h"
#include <Wt/WApplication.h>
#include <Wt/WRandom.h>

namespace Stylus
{

    CssFilesManager::CssFilesManager(std::shared_ptr<Brain> brain)
        : brain_(brain),
        FilesManager("../stylus-resources/tailwind4/css/", "css", brain->state_.css_node_->IntAttribute("sidebar-width"))
    {

        brain_->css_folders_ = folders_;
        file_saved().connect(this, [=](Wt::WString ile_path)
        {
            brain_->generateCssFile();
        });
        
        getFolders(); // to activate the signal and set the initial folders in brain_

        sidebar_->width_changed_.connect(this, [=](Wt::WString width)
        {
            brain_->state_.css_node_->SetAttribute("sidebar-width", std::stoi(width.toUTF8()));
            brain_->state_.doc_.SaveFile(brain_->state_.file_path_.c_str());
        });
        
    }

}