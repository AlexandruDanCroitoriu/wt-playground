#include "101-Stylus/001-JsFilesManager/JsFilesManager.h"
#include <Wt/WApplication.h>
#include <Wt/WRandom.h>

namespace Stylus
{

    JsFilesManager::JsFilesManager(std::shared_ptr<Brain> brain)
        : FilesManager("../stylus-resources/js/", "javascript"),
          brain_(brain)
    {

        file_saved().connect(this, [=](Wt::WString file_path)
        {
                Wt::WApplication::instance()->require(file_path.toUTF8() + "?v=" + Wt::WRandom::generateId());
        });
    }
}