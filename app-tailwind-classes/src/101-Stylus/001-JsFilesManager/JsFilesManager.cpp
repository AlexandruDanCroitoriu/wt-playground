#include "101-Stylus/001-JsFilesManager/JsFilesManager.h"
#include <Wt/WApplication.h>
#include <Wt/WRandom.h>

namespace Stylus
{

    JsFilesManager::JsFilesManager(std::shared_ptr<Brain> brain)
        : brain_(brain),
        FilesManager("../stylus-resources/js/", "javascript", brain->state_.js_node_->IntAttribute("sidebar-width"))
    {

        file_saved().connect(this, [=](Wt::WString file_path)
        {
                Wt::WApplication::instance()->require(file_path.toUTF8() + "?v=" + Wt::WRandom::generateId());
        });

        sidebar_->width_changed_.connect(this, [=](Wt::WString width)
        {
            brain_->state_.js_node_->SetAttribute("sidebar-width", std::stoi(width.toUTF8()));
            brain_->state_.doc_.SaveFile(brain_->state_.file_path_.c_str());
        });
    }
}