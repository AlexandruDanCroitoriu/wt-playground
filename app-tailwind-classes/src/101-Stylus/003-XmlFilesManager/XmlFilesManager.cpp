#include "101-Stylus/003-XmlFilesManager/XmlFilesManager.h"
#include <Wt/WApplication.h>
#include <Wt/WRandom.h>

namespace Stylus
{

    XmlFilesManager::XmlFilesManager(std::shared_ptr<Brain> brain)
        : FilesManager("../stylus-resources/xml-templates/", "xml"),
          brain_(brain)
    {

        // file_saved().connect(this, [=](Wt::WString ile_path)
        // {
        // });
    }
}