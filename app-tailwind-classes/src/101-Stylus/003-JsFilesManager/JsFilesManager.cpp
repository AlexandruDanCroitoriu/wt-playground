#include "101-Stylus/003-JsFilesManager/JsFilesManager.h"
#include <Wt/WApplication.h>
#include <Wt/WRandom.h>

namespace Stylus
{

    JsFilesManager::JsFilesManager(std::shared_ptr<StylusState> state)
        : state_(state),
        FilesManager("../stylus-resources/js/", "javascript", state->js_node_->IntAttribute("sidebar-width"))
    {        

        // file_saved().connect(this, [=](Wt::WString file_path)
        // {
        //     Wt::WApplication::instance()->require(file_path.toUTF8() + "?v=" + Wt::WRandom::generateId());
        // });

        sidebar_->width_changed_.connect(this, [=](Wt::WString width)
        {
            state_->js_node_->SetAttribute("sidebar-width", std::stoi(width.toUTF8()));
            state_->doc_.SaveFile(state_->file_path_.c_str());
        });
    }
}