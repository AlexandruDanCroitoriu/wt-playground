#include "101-Stylus/002-CssFilesManager/CssFilesManager.h"
#include <Wt/WApplication.h>
#include <Wt/WRandom.h>

namespace Stylus
{

    CssFilesManager::CssFilesManager(std::shared_ptr<StylusState> state)
        : state_(state),
        FilesManager("../stylus-resources/tailwind4/css/", "css", state->css_node_->IntAttribute("sidebar-width"), state->css_node_->Attribute("selected-file-path"))
    {
        

        sidebar_->width_changed_.connect(this, [=](Wt::WString width)
        {
            state_->css_node_->SetAttribute("sidebar-width", std::stoi(width.toUTF8()));
            state_->doc_.SaveFile(state_->file_path_.c_str());
        });

        file_selected().connect(this, [=](Wt::WString file_path)
        {
            state_->css_node_->SetAttribute("selected-file-path", file_path.toUTF8().c_str());
            state_->doc_.SaveFile(state_->file_path_.c_str());
        });
        
    }

}